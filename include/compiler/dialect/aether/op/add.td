#ifndef AETHER_OP_ADD_TD
#define AETHER_OP_ADD_TD

def Aether_AddOp : Op<Aether_Dialect, "add",
    [Pure, AllTypesMatch<["lhs", "rhs", "result"]>]> {
  let summary = "Element-wise addition of two tensors";
  let description = [{
    Computes element-wise addition of two tensors of matching shape and type.
    Semantics follow ONNX Add (opset 14): inputs must have the same shape;
    no implicit broadcasting.
  }];

  let arguments = (ins AnyTensor:$lhs, AnyTensor:$rhs);
  let results  = (outs AnyTensor:$result);

  let assemblyFormat = "$lhs `,` $rhs attr-dict `:` type($result)";
}

#endif // AETHER_OP_ADD_TD
