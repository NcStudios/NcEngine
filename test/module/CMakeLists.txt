### Random Tests ###
add_executable(ModuleRegistry_unit_tests
    ModuleRegistry_unit_tests.cpp
    ${PROJECT_SOURCE_DIR}/source/engine/module/Module.cpp
)

target_include_directories(ModuleRegistry_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ModuleRegistry_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ModuleRegistry_unit_tests
    PRIVATE
        gtest
        Taskflow
)

add_test(ModuleRegistry_unit_tests ModuleRegistry_unit_tests)

### TaskGraph Tests ###
add_executable(TaskGraph_unit_tests
    TaskGraph_unit_tests.cpp
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
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(TaskGraph_unit_tests
    PRIVATE
        gtest
        Taskflow
)

add_test(TaskGraph_unit_tests TaskGraph_unit_tests)