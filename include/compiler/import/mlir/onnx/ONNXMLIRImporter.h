#pragma once

#include <filesystem>
#include <string_view>

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>

#include "compiler/import/mlir/MLIRImporter.h"


namespace aetherweave {

class ONNXMLIRImporter : public MLIRImporter {
  public:
    std::string_view get_name() const override;

  protected:
    bool                              is_supported_extension(const std::filesystem::path& extension) const override;
    mlir::OwningOpRef<mlir::ModuleOp> run(mlir::MLIRContext& context, const std::filesystem::path& path) const override;
};

} // namespace aetherweave
