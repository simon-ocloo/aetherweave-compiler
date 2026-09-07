#pragma once

#include <filesystem>

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>


namespace aetherweave {

class MLIRImporter {
  public:
    static mlir::OwningOpRef<mlir::ModuleOp> from_file(mlir::MLIRContext& context, const std::filesystem::path& path);

    virtual ~MLIRImporter() = default;

  protected:
    virtual bool                              is_supported_extension(const std::filesystem::path& extension) = 0;
    virtual mlir::OwningOpRef<mlir::ModuleOp> run(mlir::MLIRContext& context, const std::filesystem::path& path) = 0;
};

} // namespace aetherweave
