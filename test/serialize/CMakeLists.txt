### BinaryBuffer Tests ###
add_executable(BinaryBuffer_tests
    BinaryBuffer_tests.cpp
    ${PROJECT_SOURCE_DIR}/source/engine/serialize/BinaryBuffer.cpp
)

target_include_directories(BinaryBuffer_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(BinaryBuffer_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(BinaryBuffer_tests
    PRIVATE
        gtest_main
        NcUtility
)

add_test(BinaryBuffer_tests BinaryBuffer_tests)

### BinarySerialization Tests ###
add_executable(BinarySerialization_tests
    BinarySerialization_tests.cpp
    ${PROJECT_SOURCE_DIR}/source/engine/serialize/BinaryBuffer.cpp
)

target_include_directories(BinarySerialization_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(BinarySerialization_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(BinarySerialization_tests
    PRIVATE
        gtest_main
        NcUtility
)

add_test(BinarySerialization_tests BinarySerialization_tests)
