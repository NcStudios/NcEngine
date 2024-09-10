### ContactListener Tests ###
add_executable(ContactListener_integration_tests
    ContactListener_integration_tests.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/JoltApi.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/ContactListener.cpp
)

target_include_directories(ContactListener_integration_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(ContactListener_integration_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ContactListener_integration_tests
    PRIVATE
        NcMath
        NcUtility
        Jolt
        gtest_main
)

add_test(ContactListener_integration_tests ContactListener_integration_tests)

### JoltApi Tests ###
add_executable(JoltApi_unit_tests
    JoltApi_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/JoltApi.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/ContactListener.cpp
)

target_include_directories(JoltApi_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(JoltApi_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(JoltApi_unit_tests
    PRIVATE
        NcMath
        NcUtility
        Jolt
        gtest_main
)

add_test(JoltApi_unit_tests JoltApi_unit_tests)

### Jolt Conversion Tests ###
add_executable(JoltConversion_unit_tests
    JoltConversion_unit_tests.cpp
)

target_include_directories(JoltConversion_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(JoltConversion_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(JoltConversion_unit_tests
    PRIVATE
        NcUtility
        NcMath
        Jolt
        gtest_main
)

add_test(JoltConversion_unit_tests JoltConversion_unit_tests)

### Layers Tests ###
add_executable(Layers_unit_tests
    Layers_unit_tests.cpp
)

target_include_directories(Layers_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(Layers_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Layers_unit_tests
    PRIVATE
        NcUtility
        Jolt
        gtest_main
)

add_test(Layers_unit_tests Layers_unit_tests)

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
        Jolt
        gtest_main
)

add_test(PhysicsAllocator_unit_tests PhysicsAllocator_unit_tests)

### ShapeFactory Tests ###
add_executable(ShapeFactory_unit_tests
    ShapeFactory_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/ContactListener.cpp
    ${NC_SOURCE_DIR}/physics2/jolt/JoltApi.cpp
)

target_include_directories(ShapeFactory_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(ShapeFactory_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ShapeFactory_unit_tests
    PRIVATE
        NcMath
        NcUtility
        Jolt
        gtest_main
)

add_test(ShapeFactory_unit_tests ShapeFactory_unit_tests)
