### ExceptionContext Tests ###
add_executable(ExceptionContext_unit_tests
    ExceptionContext_unit_tests.cpp
)

target_include_directories(ExceptionContext_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ExceptionContext_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ExceptionContext_unit_tests
    PRIVATE
        gtest_main
)

add_test(ExceptionContext_unit_tests ExceptionContext_unit_tests)

### Executor Tests ###
add_executable(Executor_unit_tests
    Executor_unit_tests.cpp
    ${NC_SOURCE_DIR}/task/AsyncDispatcher.cpp
    ${NC_SOURCE_DIR}/task/Executor.cpp
)

target_include_directories(Executor_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(Executor_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Executor_unit_tests
    PRIVATE
        gtest_main
        NcUtility
        Taskflow
)

add_test(Executor_unit_tests Executor_unit_tests)

### TaskGraph Tests ###
add_executable(TaskGraph_unit_tests
    TaskGraph_unit_tests.cpp
)

target_include_directories(TaskGraph_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(TaskGraph_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(TaskGraph_unit_tests
    PRIVATE
        gtest_main
        NcUtility
        Taskflow
)

add_test(TaskGraph_unit_tests TaskGraph_unit_tests)
