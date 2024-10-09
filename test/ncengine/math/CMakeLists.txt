### Random Tests ###
add_executable(Random_unit_tests
    Random_unit_tests.cpp
)

target_include_directories(Random_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(Random_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Random_unit_tests
    PRIVATE
        NcMath
        Taskflow
        gtest
)

add_test(Random_unit_tests Random_unit_tests)
