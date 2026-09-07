add_executable(aw-compile main.cpp)
target_link_libraries(aw-compile PRIVATE AetherWeaveCompiler)
install(TARGETS aw-compile DESTINATION bin)
