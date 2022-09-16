### Math Tests ###
add_executable(Math_unit_tests
    Math_unit_tests.cpp
)

target_include_directories(Math_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Math_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Math_unit_tests
    PRIVATE
        gtest
)

add_test(Math_unit_tests Math_unit_tests)

### Random Tests ###
add_executable(Random_unit_tests
    Random_unit_tests.cpp
)

target_include_directories(Random_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Random_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Random_unit_tests
    PRIVATE
        gtest
        Taskflow
)

add_test(Random_unit_tests Random_unit_tests)

### Vector Tests ###
add_executable(Vector_unit_tests
    Vector_unit_tests.cpp
)

target_include_directories(Vector_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Vector_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Vector_unit_tests
    PRIVATE
        gtest
)

add_test(Vector_unit_tests Vector_unit_tests)