### TaskGraph Tests ###
add_executable(TaskGraph_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/task/TaskGraph_unit_tests.cpp
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
)

add_test(TaskGraph_unit_tests TaskGraph_unit_tests)