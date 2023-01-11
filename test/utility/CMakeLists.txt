### Signal Tests ###
add_executable(Signal_unit_tests
    Signal_unit_tests.cpp
)

target_include_directories(Signal_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
)

target_compile_options(Signal_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Signal_unit_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(Signal_unit_tests Signal_unit_tests)