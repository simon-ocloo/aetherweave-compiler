add_subdirectory(dialect)
find_package(ONNX REQUIRED)
add_mlir_library(AetherWeaveCompiler
  import/configuration/ConfigurationImporter.cpp
  import/configuration/JSONConfigurationImporter.cpp
  import/mlir/MLIRImporter.cpp
  import/mlir/onnx/ONNXMLIRImporter.cpp
  import/mlir/onnx/op_emitter/OpEmitter.cpp
  import/mlir/onnx/op_emitter/AddOpEmitter.cpp
  pass/conversion/aether_to_linalg/AetherToLinalg.cpp
  pass/conversion/aether_to_linalg/pattern/AddOpPattern.cpp
  DEPENDS
    AetherDialectIncGen
  LINK_COMPONENTS
    Core
    Support
  LINK_LIBS PUBLIC
    AetherDialect
    MLIRArithDialect
    MLIRFuncDialect
    MLIRIR
    MLIRLinalgDialect
    MLIRPass
    MLIRTensorDialect
    MLIRTransformUtils
    ONNX::onnx
    ONNX::onnx_proto
)
