#pragma once

#include <filesystem>
#include <string_view>

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>


namespace aetherweave {

class MLIRImporter {
  public:
    static mlir::OwningOpRef<mlir::ModuleOp> from_file(mlir::MLIRContext& context, const std::filesystem::path& path);

    virtual std::string_view get_name() const = 0;
    virtual ~MLIRImporter() = default;

  protected:
    virtual bool                              is_supported_extension(const std::filesystem::path& extension) const = 0;
    virtual mlir::OwningOpRef<mlir::ModuleOp> run(mlir::MLIRContext&           context,
                                                  const std::filesystem::path& path) const = 0;
};

} // namespace aetherweave
