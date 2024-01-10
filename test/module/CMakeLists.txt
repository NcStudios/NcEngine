### ModuleRegistry Tests ###
add_executable(ModuleRegistry_unit_tests
    ModuleRegistry_unit_tests.cpp
)

target_include_directories(ModuleRegistry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(ModuleRegistry_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ModuleRegistry_unit_tests
    PRIVATE
        gtest_main
        Taskflow
        NcUtility
)

add_test(ModuleRegistry_unit_tests ModuleRegistry_unit_tests)
