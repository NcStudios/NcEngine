### CameraSubsystem Tests ###
add_executable(CameraSubsystem_tests
    CameraSubsystem_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/CameraSubsystem.cpp
    ${NC_SOURCE_DIR}/graphics/components/Camera.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(CameraSubsystem_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(CameraSubsystem_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(CameraSubsystem_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(CameraSubsystem_tests CameraSubsystem_tests)

### InstanceCache Tests ###
add_executable(InstanceCache_tests
    InstanceCache_tests.cpp
)

target_include_directories(InstanceCache_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(InstanceCache_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(InstanceCache_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(InstanceCache_tests InstanceCache_tests)

### MaterialPassCache Tests ###
add_executable(MaterialPassCache_tests
    MaterialPassCache_tests.cpp
)

target_include_directories(MaterialPassCache_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(MaterialPassCache_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(MaterialPassCache_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(MaterialPassCache_tests MaterialPassCache_tests)

### MaterialRegistry Tests ###
add_executable(MaterialRegistry_tests
    MaterialRegistry_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/Material.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/MaterialRegistry.cpp
)

target_include_directories(MaterialRegistry_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(MaterialRegistry_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(MaterialRegistry_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(MaterialRegistry_tests MaterialRegistry_tests)

### MeshRendererSubsystem Tests ###
add_executable(MeshRendererSubsystem_tests
    MeshRendererSubsystem_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/graphics2/MeshRenderer2.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/MeshRendererSubsystem.cpp
)

target_include_directories(MeshRendererSubsystem_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(MeshRendererSubsystem_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(MeshRendererSubsystem_tests
    PRIVATE
        gtest_main
        NcAsset
        NcMath
)

add_test(MeshRendererSubsystem_tests MeshRendererSubsystem_tests)


