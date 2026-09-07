#include <string_view>

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Value.h>
#include <onnx/onnx-ml.pb.h>

#include "compiler/dialect/aether/op/add.h"
#include "compiler/import/mlir/onnx/op_emitter/AddOpEmitter.h"


namespace aetherweave {

bool AddOpEmitter::is_supported_node(std::string_view op_type)
{
    return op_type == "Add";
}


bool AddOpEmitter::run(const onnx::NodeProto& node, mlir::OpBuilder& builder, llvm::StringMap<mlir::Value>& value_map)
{
    if (node.input_size() < 2 || node.output_size() < 1) {
        llvm::errs() << "AddOpEmitter: malformed Add node\n";
        return false;
    }

    mlir::Value lhs = value_map.lookup(node.input(0));
    mlir::Value rhs = value_map.lookup(node.input(1));
    if (lhs == mlir::Value{} || rhs == mlir::Value{}) {
        llvm::errs() << "AddOpEmitter: undefined input in Add node\n";
        return false;
    }

    auto location = mlir::UnknownLoc::get(builder.getContext());
    value_map[node.output(0)] = builder.create<AddOp>(location, lhs.getType(), lhs, rhs).getResult();

    return true;
}

} // namespace aetherweave
