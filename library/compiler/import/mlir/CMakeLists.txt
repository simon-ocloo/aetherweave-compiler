find_package(ONNX REQUIRED)

add_mlir_library(AetherMLIRImport
  MLIRImporter.cpp
  onnx/ONNXMLIRImporter.cpp
  onnx/op_emitter/OpEmitter.cpp
  onnx/op_emitter/AddOpEmitter.cpp
  DEPENDS
    AetherDialectIncGen
  LINK_LIBS PUBLIC
    AetherDialect
    MLIRIR
    MLIRFuncDialect
    ONNX::onnx
    ONNX::onnx_proto
)
