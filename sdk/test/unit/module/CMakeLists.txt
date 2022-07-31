### Random Tests ###
add_executable(ModuleRegistry_tests
    ModuleRegistry_tests.cpp
    ${PROJECT_SOURCE_DIR}/sdk/engine/source/engine/Module.cpp
)

target_include_directories(ModuleRegistry_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ModuleRegistry_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ModuleRegistry_tests
    PRIVATE
        gtest
        Taskflow
)

add_test(ModuleRegistry_tests ModuleRegistry_tests)