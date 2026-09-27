#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/IR/Value.h>
#include <onnx/onnx-ml.pb.h>

#include "compiler/import/mlir/onnx/ONNXMLIRImporter.h"
#include "compiler/import/mlir/onnx/op_emitter/OpEmitter.h"


namespace aetherweave {

std::string_view ONNXMLIRImporter::get_name() const
{
    return "ONNXMLIRImporter";
}


bool ONNXMLIRImporter::is_supported_extension(const std::filesystem::path& extension) const
{
    return extension == ".onnx";
}


namespace {

std::optional<onnx::ModelProto> load_model(const std::filesystem::path& path)
{
    std::ifstream input_file(path, std::ios::binary);
    if (input_file.is_open() == false) {
        llvm::errs() << "ONNXMLIRImporter: cannot open '" << path.string() << "'\n";
        return std::nullopt;
    }

    onnx::ModelProto model;
    if (model.ParseFromIstream(&input_file) == false) {
        llvm::errs() << "ONNXMLIRImporter: failed to parse '" << path.string() << "'\n";
        return std::nullopt;
    }
    return model;
}


struct TensorInfo {
    std::string      name;
    mlir::TensorType type;
};


mlir::Type to_mlir_type(mlir::MLIRContext& context, int element_type)
{
    switch (element_type) {
    case onnx::TensorProto::FLOAT:
        return mlir::Float32Type::get(&context);
    case onnx::TensorProto::DOUBLE:
        return mlir::Float64Type::get(&context);
    case onnx::TensorProto::INT32:
        return mlir::IntegerType::get(&context, 32);
    case onnx::TensorProto::INT64:
        return mlir::IntegerType::get(&context, 64);
    default:
        llvm::errs() << "ONNXMLIRImporter: unsupported element type " << element_type << "\n";
        return {};
    }
}


mlir::TensorType to_mlir_tensor_type(mlir::MLIRContext& context, const onnx::TypeProto_Tensor& proto)
{
    mlir::Type element_type = to_mlir_type(context, proto.elem_type());
    if (element_type == mlir::Type{}) {
        return {};
    }
    if (proto.has_shape() == false) {
        return mlir::UnrankedTensorType::get(element_type);
    }

    llvm::SmallVector<int64_t> dimensions;
    for (const auto& dimension : proto.shape().dim()) {
        dimensions.push_back(dimension.has_dim_value() ? dimension.dim_value() : mlir::ShapedType::kDynamic);
    }

    return mlir::RankedTensorType::get(dimensions, element_type);
}


template <typename TensorRange>
std::optional<llvm::SmallVector<TensorInfo>> resolve_tensors(const TensorRange& tensors,
                                                             std::string_view   role,
                                                             mlir::MLIRContext& context)
{
    llvm::SmallVector<TensorInfo> result;
    for (const auto& info : tensors) {
        if (info.type().has_tensor_type() == false) {
            llvm::errs() << "ONNXMLIRImporter: non-tensor " << role << " '" << info.name() << "'\n";
            return std::nullopt;
        }
        auto tensor_type = to_mlir_tensor_type(context, info.type().tensor_type());
        if (tensor_type == mlir::TensorType{}) {
            llvm::errs() << "ONNXMLIRImporter: unsupported type for " << role << " '" << info.name() << "'\n";
            return std::nullopt;
        }
        result.push_back({info.name(), tensor_type});
    }
    return result;
}


bool import_graph(const onnx::GraphProto& graph, mlir::ModuleOp mlir_module)
{
    mlir::MLIRContext& context = *mlir_module.getContext();
    auto               location = mlir::UnknownLoc::get(&context);

    auto inputs = resolve_tensors(graph.input(), "input", context);
    if (inputs.has_value() == false) {
        return false;
    }

    auto outputs = resolve_tensors(graph.output(), "output", context);
    if (outputs.has_value() == false) {
        return false;
    }

    llvm::SmallVector<mlir::Type> input_types;
    for (const auto& info : *inputs) {
        input_types.push_back(info.type);
    }

    llvm::SmallVector<mlir::Type> output_types;
    for (const auto& info : *outputs) {
        output_types.push_back(info.type);
    }

    mlir::OpBuilder module_builder(mlir_module.getBodyRegion());
    module_builder.setInsertionPointToEnd(mlir_module.getBody());

    auto function_type = mlir::FunctionType::get(&context, input_types, output_types);
    auto function = module_builder.create<mlir::func::FuncOp>(location, "main", function_type);

    auto*           entry_block = function.addEntryBlock();
    mlir::OpBuilder builder(entry_block, entry_block->end());

    llvm::StringMap<mlir::Value> value_map;
    for (auto [info, arg] : llvm::zip(*inputs, entry_block->getArguments())) {
        value_map[info.name] = arg;
    }

    for (const auto& node : graph.node()) {
        if (OpEmitter::from_node(node, builder, value_map) == false) {
            return false;
        }
    }

    llvm::SmallVector<mlir::Value> return_values;
    for (const auto& info : *outputs) {
        mlir::Value output_value = value_map.lookup(info.name);
        if (output_value == mlir::Value{}) {
            llvm::errs() << "ONNXMLIRImporter: output '" << info.name << "' not found\n";
            return false;
        }
        return_values.push_back(output_value);
    }
    builder.create<mlir::func::ReturnOp>(location, return_values);

    return true;
}

} // namespace


mlir::OwningOpRef<mlir::ModuleOp> ONNXMLIRImporter::run(mlir::MLIRContext&           context,
                                                        const std::filesystem::path& path) const
{
    auto model = load_model(path);
    if (model.has_value() == false) {
        return {};
    }

    auto mlir_module = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));
    if (import_graph(model->graph(), mlir_module) == false) {
        return {};
    }
    mlir_module->setAttr("debug.origin", mlir::StringAttr::get(&context, this->get_name()));
    return mlir_module;
}

} // namespace aetherweave
