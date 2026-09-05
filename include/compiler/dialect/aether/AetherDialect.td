#ifndef AETHER_DIALECT_TD
#define AETHER_DIALECT_TD

include "mlir/IR/BuiltinDialect.td"
include "mlir/IR/OpBase.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def Aether_Dialect : Dialect {
  let name = "aether";
  let summary = "Hardware-agnostic ML IR dialect";
  let description = [{
    The aether dialect represents neural network operations in a
    hardware-agnostic form. It stays abstract until the final lowering
    step, where target-specific passes introduce hardware details.
  }];
  let cppNamespace = "::aetherweave";
}

include "op/add.td"

#endif // AETHER_DIALECT_TD
