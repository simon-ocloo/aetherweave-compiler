add_executable(aw-compile main.cpp)
target_link_libraries(aw-compile PRIVATE AetherMLIRImport AetherConfigurationImport)
install(TARGETS aw-compile DESTINATION bin)
