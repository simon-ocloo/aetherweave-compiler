add_mlir_library(AetherDialect
  AetherDialect.cpp
  DEPENDS
    AetherDialectIncGen
  LINK_LIBS PUBLIC
    MLIRIR
)
