### StepTimer Tests ###
add_executable(StepTimer_unit_tests
    StepTimer_unit_tests.cpp
)

target_include_directories(StepTimer_unit_tests
    PRIVATE
        ${NC_SOURCE_DIR}
)

target_compile_options(StepTimer_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(StepTimer_unit_tests
    PRIVATE
        gtest_main
)

add_test(StepTimer_unit_tests StepTimer_unit_tests)
