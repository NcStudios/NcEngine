### Math Tests ###
add_executable(Math_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/math/Math_unit_tests.cpp
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
        ${WARNING_FLAGS}
)

target_link_libraries(Math_unit_tests
    PRIVATE
        gtest
)

add_test(Math_unit_tests Math_unit_tests)