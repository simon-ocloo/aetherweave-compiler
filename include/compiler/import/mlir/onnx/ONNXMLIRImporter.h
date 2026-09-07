#pragma once

#include <filesystem>

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>

#include "compiler/import/mlir/MLIRImporter.h"


namespace aetherweave {

class ONNXMLIRImporter : public MLIRImporter {
  protected:
    bool                              is_supported_extension(const std::filesystem::path& extension) override;
    mlir::OwningOpRef<mlir::ModuleOp> run(mlir::MLIRContext& context, const std::filesystem::path& path) override;
};

} // namespace aetherweave
