#pragma once

#include <filesystem>
#include <optional>

#include <mlir/IR/Operation.h>
#include <mlir/Pass/Pass.h>
#include <mlir/Pass/PassInstrumentation.h>


namespace aetherweave {

class DebugPassInstrumentation : public mlir::PassInstrumentation {
  public:
    DebugPassInstrumentation(const std::filesystem::path& directory_path, mlir::Operation* initial_operation);

    void runAfterPass(mlir::Pass* pass, mlir::Operation* operation) override;
    void runAfterPassFailed(mlir::Pass* pass, mlir::Operation* operation) override;
    void runBeforePass(mlir::Pass* pass, mlir::Operation* operation) override;

  private:
    std::optional<std::filesystem::path> snapshot_directory_path;
    int                                  step;
};

} // namespace aetherweave
