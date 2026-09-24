#include <cstdint>

#include <llvm/ADT/SmallVector.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/TypeRange.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/ValueRange.h>
#include <mlir/Support/LogicalResult.h>
#include <mlir/Transforms/DialectConversion.h>

#include "compiler/dialect/aether/AetherOps.h"
#include "compiler/pass/conversion/aether_to_linalg/pattern/AddOpPattern.h"


namespace aetherweave {

mlir::LogicalResult AddOpPattern::matchAndRewrite(AddOp                            op,
                                                  OpAdaptor                        adaptor,
                                                  mlir::ConversionPatternRewriter& rewriter) const
{
    auto loc = op.getLoc();
    auto result_type = mlir::dyn_cast<mlir::RankedTensorType>(op.getResult().getType());
    if (result_type == mlir::RankedTensorType{}) {
        return rewriter.notifyMatchFailure(op, "expected a ranked tensor result");
    }

    llvm::SmallVector<mlir::Value> dynamic_dims;
    for (int64_t i = 0; i < result_type.getRank(); ++i) {
        if (result_type.isDynamicDim(i)) {
            dynamic_dims.push_back(rewriter.create<mlir::tensor::DimOp>(loc, adaptor.getLhs(), i));
        }
    }

    mlir::Value init = rewriter.create<mlir::tensor::EmptyOp>(loc, result_type, dynamic_dims);

    rewriter.replaceOpWithNewOp<mlir::linalg::AddOp>(
        op, mlir::TypeRange{result_type}, mlir::ValueRange{adaptor.getLhs(), adaptor.getRhs()}, mlir::ValueRange{init});

    return mlir::success();
}

} // namespace aetherweave
