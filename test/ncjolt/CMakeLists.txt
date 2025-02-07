### ByteArrayStream Tests ###
add_executable(ByteArrayStream_unit_tests
    ByteArrayStream_unit_tests.cpp
)

target_include_directories(ByteArrayStream_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(ByteArrayStream_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ByteArrayStream_unit_tests
    PRIVATE
        NcJolt
        gtest_main
)

add_test(ByteArrayStream_unit_tests ByteArrayStream_unit_tests)

### PhysicsAllocator Tests ###
add_executable(PhysicsAllocator_unit_tests
    PhysicsAllocator_unit_tests.cpp
)

target_include_directories(PhysicsAllocator_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(PhysicsAllocator_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PhysicsAllocator_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(PhysicsAllocator_unit_tests PhysicsAllocator_unit_tests)
