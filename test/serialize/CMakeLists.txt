### ComponentSerialization Tests ###
add_executable(ComponentSerialization_integration_tests
    ComponentSerialization_integration_tests.cpp
    ${NC_SOURCE_DIR}/serialize/ComponentSerialization.cpp
    ${NC_SOURCE_DIR}/audio/AudioSource.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/graphics/components/MeshRenderer.cpp
    ${NC_SOURCE_DIR}/graphics/components/ParticleEmitter.cpp
    ${NC_SOURCE_DIR}/graphics/components/ToonRenderer.cpp
    ${NC_SOURCE_DIR}/physics/Collider.cpp
    ${NC_SOURCE_DIR}/physics/ConcaveCollider.cpp
    ${NC_SOURCE_DIR}/physics/PhysicsBody.cpp
)

target_include_directories(ComponentSerialization_integration_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(ComponentSerialization_integration_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ComponentSerialization_integration_tests
    PRIVATE
        NcAsset
        NcMath
        gtest_main
)

add_test(ComponentSerialization_integration_tests ComponentSerialization_integration_tests)

### SceneSerialization Tests ###
add_executable(SceneSerialization_unit_tests
    SceneSerialization_unit_tests.cpp
    ${NC_SOURCE_DIR}/serialize/SceneSerialization.cpp
    ${NC_SOURCE_DIR}/serialize/EntitySerializationUtility.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(SceneSerialization_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
)

target_compile_options(SceneSerialization_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SceneSerialization_unit_tests
    PRIVATE
        NcAsset
        NcMath
        gtest_main
)

add_test(SceneSerialization_unit_tests SceneSerialization_unit_tests)
