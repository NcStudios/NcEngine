### Config Tests ###
add_executable(Config_tests
    Config_tests.cpp
    ${NC_SOURCE_DIR}/config/Config.cpp
)

target_include_directories(Config_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(Config_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_compile_definitions(Config_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/collateral"
)

target_link_libraries(Config_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(Config_tests Config_tests)
