add_mlir_library(AetherConfigurationImport
  ConfigurationImporter.cpp
  JSONConfigurationImporter.cpp
  LINK_LIBS PUBLIC
    LLVMSupport
)
