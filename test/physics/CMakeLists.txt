### Contacts Tests ###
add_executable(Constraints_unit_tests
    Constraints_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/Constraints.cpp
)

target_include_directories(Constraints_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(Constraints_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Constraints_unit_tests
    PRIVATE
        NcMath
        NcUtility
        Jolt
        gtest_main
)

add_test(Constraints_unit_tests Constraints_unit_tests)

### Contacts Tests ###
add_executable(Contacts_unit_tests
    Contacts_unit_tests.cpp
)

target_include_directories(Contacts_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(Contacts_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Contacts_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(Contacts_unit_tests Contacts_unit_tests)

### PhysicsUtility Tests ###
add_executable(PhysicsUtility_unit_tests
    PhysicsUtility_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/PhysicsUtility.cpp
)

target_include_directories(PhysicsUtility_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(PhysicsUtility_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PhysicsUtility_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(PhysicsUtility_unit_tests PhysicsUtility_unit_tests)

### RigidBody Tests ###
add_executable(RigidBody_unit_tests
    RigidBody_unit_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/physics/RigidBody.cpp
    ${NC_SOURCE_DIR}/physics/Shape.cpp
    ${NC_SOURCE_DIR}/physics/jolt/BodyFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintFactory.cpp
    ${NC_SOURCE_DIR}/physics/jolt/ConstraintManager.cpp
    ${NC_SOURCE_DIR}/physics/jolt/JoltApi.cpp
)

target_include_directories(RigidBody_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(RigidBody_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(RigidBody_unit_tests
    PRIVATE
        NcMath
        NcUtility
        Jolt
        gtest_main
)

add_test(RigidBody_unit_tests RigidBody_unit_tests)

### Shape Tests ###
add_executable(Shape_unit_tests
    Shape_unit_tests.cpp
    ${NC_SOURCE_DIR}/physics/Shape.cpp
)

target_include_directories(Shape_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(Shape_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(Shape_unit_tests
    PRIVATE
        NcMath
        NcUtility
        gtest_main
)

add_test(Shape_unit_tests Shape_unit_tests)

add_subdirectory(jolt)
