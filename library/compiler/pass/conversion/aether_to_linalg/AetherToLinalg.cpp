#include <memory>
#include <utility>

#include <llvm/ADT/StringRef.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/IR/PatternMatch.h>
#include <mlir/Pass/Pass.h>
#include <mlir/Support/LogicalResult.h>
#include <mlir/Transforms/DialectConversion.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/pass/conversion/aether_to_linalg/AetherToLinalg.h"
#include "compiler/pass/conversion/aether_to_linalg/pattern/AddOpPattern.h"


namespace aetherweave {

mlir::TypeID AetherToLinalgPass::resolveTypeID()
{
    static mlir::TypeID id =
        mlir::TypeID::getFromOpaquePointer(reinterpret_cast<const void*>(&AetherToLinalgPass::resolveTypeID));
    return id;
}


AetherToLinalgPass::AetherToLinalgPass() : mlir::OperationPass<mlir::ModuleOp>(mlir::TypeID::get<AetherToLinalgPass>())
{
}


std::unique_ptr<mlir::Pass> AetherToLinalgPass::clonePass() const
{
    return std::make_unique<AetherToLinalgPass>(*this);
}


llvm::StringRef AetherToLinalgPass::getArgument() const
{
    return "aether-to-linalg";
}


void AetherToLinalgPass::getDependentDialects(mlir::DialectRegistry& registry) const
{
    registry.insert<mlir::arith::ArithDialect, mlir::linalg::LinalgDialect, mlir::tensor::TensorDialect>();
}


llvm::StringRef AetherToLinalgPass::getName() const
{
    return "AetherToLinalg";
}


void AetherToLinalgPass::runOnOperation()
{
    mlir::ConversionTarget target(getContext());
    target.addIllegalDialect<AetherDialect>();
    target.addLegalDialect<mlir::arith::ArithDialect, mlir::linalg::LinalgDialect, mlir::tensor::TensorDialect>();

    mlir::RewritePatternSet patterns(&getContext());
    patterns.add<AddOpPattern>(&getContext());

    if (mlir::failed(mlir::applyPartialConversion(getOperation(), target, std::move(patterns)))) {
        signalPassFailure();
    }
}

} // namespace aetherweave
