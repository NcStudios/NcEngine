### GraphicsUtilities Tests ###
add_executable(GraphicsUtilities_tests
    GraphicsUtilities_tests.cpp
)

target_include_directories(GraphicsUtilities_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(GraphicsUtilities_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(GraphicsUtilities_tests
    PRIVATE
        gtest_main
        NcAsset
        NcMath
        NcUtility
        Taskflow
)

add_test(GraphicsUtilities_tests GraphicsUtilities_tests)