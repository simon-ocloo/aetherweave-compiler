#pragma once

#include <memory>

#include <llvm/ADT/StringRef.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/Pass/Pass.h>


namespace aetherweave {

class AetherToLinalgPass : public mlir::OperationPass<mlir::ModuleOp> {
  public:
    static mlir::TypeID resolveTypeID();

    AetherToLinalgPass();
    AetherToLinalgPass(const AetherToLinalgPass&) = default;

    std::unique_ptr<mlir::Pass> clonePass() const override;
    llvm::StringRef             getArgument() const override;
    void                        getDependentDialects(mlir::DialectRegistry& registry) const override;
    llvm::StringRef             getName() const override;
    void                        runOnOperation() override;
};

} // namespace aetherweave
