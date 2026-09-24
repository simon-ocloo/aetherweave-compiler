#pragma once

#include <mlir/Support/LogicalResult.h>
#include <mlir/Transforms/DialectConversion.h>

#include "compiler/dialect/aether/AetherOps.h"


namespace aetherweave {

class AddOpPattern : public mlir::OpConversionPattern<AddOp> {
  public:
    using mlir::OpConversionPattern<AddOp>::OpConversionPattern;

    mlir::LogicalResult matchAndRewrite(AddOp                            op,
                                        OpAdaptor                        adaptor,
                                        mlir::ConversionPatternRewriter& rewriter) const override;
};

} // namespace aetherweave
