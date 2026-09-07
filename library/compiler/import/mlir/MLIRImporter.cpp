#include <filesystem>

#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Support/LogicalResult.h>

#include "compiler/import/mlir/MLIRImporter.h"
#include "compiler/import/mlir/onnx/ONNXMLIRImporter.h"
#include "helper/handler.h"


namespace aetherweave {

mlir::OwningOpRef<mlir::ModuleOp> MLIRImporter::from_file(mlir::MLIRContext& context, const std::filesystem::path& path)
{
    static const auto handlers = helper::make_handlers<MLIRImporter, ONNXMLIRImporter>();
    for (const auto& handler : handlers) {
        if (handler->is_supported_extension(path.extension())) {
            auto mlir_module = handler->run(context, path);
            if (static_cast<bool>(mlir_module) == true and mlir::failed(mlir::verify(mlir_module->getOperation()))) {
                llvm::errs() << "MLIRImporter: '" << path.string() << "' was imported into invalid IR\n";
                return {};
            }
            return mlir_module;
        }
    }
    llvm::errs() << "MLIRImporter: unsupported format for '" << path.string() << "'\n";
    return {};
}

} // namespace aetherweave
