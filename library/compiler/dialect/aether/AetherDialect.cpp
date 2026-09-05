#include <mlir/IR/Builders.h>
#include <mlir/IR/DialectImplementation.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/dialect/aether/op/add.h"


#include "compiler/dialect/aether/AetherDialect.cpp.inc"
#define GET_OP_CLASSES
#include "compiler/dialect/aether/AetherOps.cpp.inc"

namespace aetherweave {

void AetherDialect::initialize()
{
    addOperations<
#define GET_OP_LIST
#include "compiler/dialect/aether/AetherOps.cpp.inc"
        >();
}

} // namespace aetherweave
