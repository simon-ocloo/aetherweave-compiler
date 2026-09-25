add_executable(aw-opt main.cpp)
target_link_libraries(aw-opt PRIVATE
  AetherWeaveCompiler
  MLIROptLib
)
install(TARGETS aw-opt DESTINATION bin)
