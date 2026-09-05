#pragma once

#include <mlir/Interfaces/SideEffectInterfaces.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/OpDefinition.h>


#define GET_OP_CLASSES
#include "compiler/dialect/aether/AetherOps.h.inc"
