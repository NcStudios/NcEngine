### Allocator Tests ###
add_executable(Allocator_unit_tests
    ${ROOT_DIR}/test/unit/alloc/Allocator_unit_tests.cpp
)

target_include_directories(Allocator_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(Allocator_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(Allocator_unit_tests
    PRIVATE
        gtest
)

add_test(Allocator_unit_tests Allocator_unit_tests)

### MakeUnique Tests ###
add_executable(MakeUnique_unit_tests
    ${ROOT_DIR}/test/unit/alloc/MakeUnique_unit_tests.cpp
)

target_include_directories(MakeUnique_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(MakeUnique_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(MakeUnique_unit_tests
    PRIVATE
        gtest
)

add_test(MakeUnique_unit_tests MakeUnique_unit_tests)

### MemoryResource Tests ###
add_executable(MemoryResource_unit_tests
    ${ROOT_DIR}/test/unit/alloc/MemoryResource_unit_tests.cpp
)

target_include_directories(MemoryResource_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(MemoryResource_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(MemoryResource_unit_tests
    PRIVATE
        gtest
)

add_test(MemoryResource_unit_tests MemoryResource_unit_tests)