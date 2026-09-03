### Math Tests ###
add_executable(Math_unit_tests
    Math_unit_test.cpp
)

target_include_directories(Math_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(Math_unit_tests
    PRIVATE
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Math_unit_tests
    PRIVATE
        gtest
)

add_test(Math_unit_tests Math_unit_tests)

### Quaternion Tests ###
add_executable(Quaternion_unit_tests
    Quaternion_unit_test.cpp
    ${PROJECT_SOURCE_DIR}/source/ncmath/Quaternion.cpp
)

target_include_directories(Quaternion_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(Quaternion_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Quaternion_unit_tests
    PRIVATE
        gtest_main
        fmt::fmt
        DirectXMath
)

add_test(Quaternion_unit_tests Quaternion_unit_tests)

### Vector Tests ###
add_executable(Vector_unit_tests
    Vector_unit_test.cpp
)

target_include_directories(Vector_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
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
