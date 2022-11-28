### PairCache Tests ###
add_executable(PairCache_unit_tests
    PairCache_unit_tests.cpp
)

target_include_directories(PairCache_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(PairCache_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PairCache_unit_tests
    PRIVATE
        NcMath
        gtest
)

add_test(PairCache_unit_tests PairCache_unit_tests)

### Simplex Tests ###
add_executable(Simplex_unit_tests
    Simplex_unit_tests.cpp
)

target_include_directories(Simplex_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(Simplex_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Simplex_unit_tests
    PRIVATE
        NcMath
        gtest
)

add_test(Simplex_unit_tests Simplex_unit_tests)