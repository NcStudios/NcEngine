### BodyFactory Tests ###
add_executable(BodyFactory_unit_tests
    BodyFactory_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/BodyFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ShapeFactory.cpp
    ${NC_SOURCE_DIR}/physics/Shape.cpp
)

target_include_directories(BodyFactory_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(BodyFactory_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(BodyFactory_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(BodyFactory_unit_tests BodyFactory_unit_tests)

### BodyManager Tests ###
add_executable(BodyManager_integration_tests
    BodyManager_integration_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/physics/jolt/BodyFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/BodyManager.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintManager.cpp
    ${NC_SOURCE_DIR}/physics/jolt/VehicleManager.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ShapeFactory.cpp
    ${NC_SOURCE_DIR}/physics/RigidBody.cpp
    ${NC_SOURCE_DIR}/physics/Shape.cpp
)

target_include_directories(BodyManager_integration_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(BodyManager_integration_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(BodyManager_integration_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(BodyManager_integration_tests BodyManager_integration_tests)

### CollisionQueryUtility Tests ###
add_executable(CollisionQueryUtility_unit_tests
    CollisionQueryUtility_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
)

target_include_directories(CollisionQueryUtility_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(CollisionQueryUtility_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(CollisionQueryUtility_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(CollisionQueryUtility_unit_tests CollisionQueryUtility_unit_tests)

### ContactListener Tests ###
add_executable(ContactListener_integration_tests
    ContactListener_integration_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ContactListener.cpp
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
        NcJolt
        gtest_main
)

add_test(ContactListener_integration_tests ContactListener_integration_tests)

### ConstraintFactory Tests ###
add_executable(ConstraintFactory_unit_tests
    ConstraintFactory_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
)

target_include_directories(ConstraintFactory_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(ConstraintFactory_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ConstraintFactory_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(ConstraintFactory_unit_tests ConstraintFactory_unit_tests)

### ConstraintManager Tests ###
add_executable(ConstraintManager_unit_tests
    ConstraintManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintManager.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
)

target_include_directories(ConstraintManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(ConstraintManager_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ConstraintManager_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(ConstraintManager_unit_tests ConstraintManager_unit_tests)

### JoltPhysics Tests ###
add_executable(JoltPhysics_unit_tests
    JoltPhysics_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ContactListener.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/PhysicsSnapshot.cpp
)

target_include_directories(JoltPhysics_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(JoltPhysics_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(JoltPhysics_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(JoltPhysics_unit_tests JoltPhysics_unit_tests)

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

### ShapeFactory Tests ###
add_executable(ShapeFactory_unit_tests
    ShapeFactory_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ShapeFactory.cpp
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
        NcJolt
        gtest_main
)

add_test(ShapeFactory_unit_tests ShapeFactory_unit_tests)

### VehicleManager Tests ###
add_executable(VehicleManager_unit_tests
    VehicleManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltPhysics.cpp
    ${NC_SOURCE_DIR}/physics/jolt/VehicleManager.cpp
)

target_include_directories(VehicleManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(VehicleManager_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(VehicleManager_unit_tests
    PRIVATE
        NcMath
        NcUtility
        NcJolt
        gtest_main
)

add_test(VehicleManager_unit_tests VehicleManager_unit_tests)
