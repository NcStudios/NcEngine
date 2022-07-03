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