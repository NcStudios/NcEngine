### DiligentEngineLinux_tests Tests ###
add_executable(DiligentEngineLinux_tests
    DiligentEngineLinux_tests.cpp
        ${NC_SOURCE_DIR}/config/Config.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/DiligentEngineLinux.cpp
        ${NC_SOURCE_DIR}/graphics2/NcGraphicsImpl2Linux.cpp
)

target_include_directories(DiligentEngineLinux_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${diligentcore_SOURCE_DIR}
)

target_compile_options(DiligentEngineLinux_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(DiligentEngineLinux_tests
    PRIVATE
        gtest_main
        glfw
        NcMath
        ${DILIGENT_LIBRARIES}
)

set_target_properties(DiligentEngineLinux_tests
                      PROPERTIES
                      BUILD_RPATH "$<TARGET_FILE_DIR:DiligentEngineLinux_tests>/../_deps/diligentcore-build/Graphics/GraphicsEngineOpenGL;$<TARGET_FILE_DIR:DiligentEngineLinux_tests/../_deps/diligentcore-build/Graphics/GraphicsEngineVulkan"
)

add_test(DiligentEngineLinux_tests DiligentEngineLinux_tests)