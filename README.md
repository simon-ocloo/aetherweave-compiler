# AetherWeave - Compiler

MLIR/LLVM-based ML compiler. Takes ONNX models and produces compiled artifacts (shared kernels + metadata) ready to be loaded by the executor.

Part of the [AetherWeave](https://github.com/simon-ocloo/aetherweave) project — see that repo for the full architecture, Dockerfile, and test suite.

## Tools

- `aw-compile` — compiles an ONNX model to a training or inference artifact
- `aw-opt` — standalone MLIR pass runner for development and testing (like `mlir-opt`)
