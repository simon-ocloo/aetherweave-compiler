#include <filesystem>

#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>

#include "compiler/import/mlir/MLIRImporter.h"
#include "compiler/import/mlir/onnx/ONNXMLIRImporter.h"
#include "helper/make_handlers.h"


namespace aetherweave {

mlir::OwningOpRef<mlir::ModuleOp> MLIRImporter::from_file(mlir::MLIRContext& context, const std::filesystem::path& path)
{
    auto handlers = make_handlers<MLIRImporter, ONNXMLIRImporter>();
    for (auto& handler : handlers) {
        if (handler->is_supported_extension(path.extension())) {
            return handler->run(context, path);
        }
    }
    llvm::errs() << "MLIRImporter: unsupported format for '" << path.string() << "'\n";
    return {};
}

} // namespace aetherweave
