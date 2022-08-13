### Random Tests ###
add_executable(ModuleRegistry_tests
    ModuleRegistry_tests.cpp
    ${PROJECT_SOURCE_DIR}/sdk/engine/source/module/Module.cpp
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

### TaskGraph Tests ###
add_executable(TaskGraph_unit_tests
    TaskGraph_tests.cpp
)

target_include_directories(TaskGraph_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(TaskGraph_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(TaskGraph_unit_tests
    PRIVATE
        gtest
        Taskflow
)

add_test(TaskGraph_unit_tests TaskGraph_unit_tests)