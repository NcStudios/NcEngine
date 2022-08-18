set(NC_HASH_TEST_COLLATERAL_DIRECTORY ${PROJECT_SOURCE_DIR}/test/utility/collateral/)

### StringHash Tests ###
add_executable(StringHash_unit_tests
    StringHash_unit_tests.cpp
)

target_compile_definitions(StringHash_unit_tests
    PRIVATE
        NC_HASH_TEST_COLLATERAL_DIRECTORY="${NC_HASH_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(StringHash_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(StringHash_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(StringHash_unit_tests
    PRIVATE
        gtest
)

add_test(StringHash_unit_tests StringHash_unit_tests)

### Signal Tests ###
add_executable(Signal_unit_tests
    Signal_unit_tests.cpp
)

target_include_directories(Signal_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(Signal_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(Signal_unit_tests
    PRIVATE
        gtest
)

add_test(Signal_unit_tests Signal_unit_tests)