### Transform Tests ###
add_executable(Task_unit_tests
    ${ROOT_DIR}/test/unit/task/Task_unit_tests.cpp
)

target_include_directories(Task_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${EXTERNAL_INCLUDE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Task_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Task_unit_tests
    PRIVATE
        gtest
)

add_test(Task_unit_tests Task_unit_tests)