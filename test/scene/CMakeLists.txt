### SceneFragment Tests ###
add_executable(SceneFragment_unit_tests
    SceneFragment_unit_tests.cpp
    ${NC_SOURCE_DIR}/scene/SceneFragment.cpp
    ${NC_SOURCE_DIR}/ecs/Registry.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(SceneFragment_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
)

target_compile_options(SceneFragment_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SceneFragment_unit_tests
    PRIVATE
        NcAsset
        NcMath
        gtest_main
)

add_test(SceneFragment_unit_tests SceneFragment_unit_tests)
