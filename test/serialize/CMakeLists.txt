### SceneSerialization Tests ###
add_executable(SceneSerialization_unit_tests
    SceneSerialization_unit_tests.cpp
    ${NC_SOURCE_DIR}/serialize/SceneSerialization.cpp
    ${NC_SOURCE_DIR}/serialize/EntitySerializationUtility.cpp
    ${NC_SOURCE_DIR}/ecs/Registry.cpp
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
