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
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/InstanceCache.cpp
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

### GraphicsUtility Tests ###
add_executable(GraphicsUtility_tests
    GraphicsUtility_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/GraphicsUtility.cpp
)

target_include_directories(GraphicsUtility_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(GraphicsUtility_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(GraphicsUtility_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(GraphicsUtility_tests GraphicsUtility_tests)

### HostStructuredBuffer Tests ###
add_executable(HostStructuredBuffer_tests
    HostStructuredBuffer_tests.cpp
)

target_include_directories(HostStructuredBuffer_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(HostStructuredBuffer_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(HostStructuredBuffer_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(HostStructuredBuffer_tests HostStructuredBuffer_tests)

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
    ${NC_SOURCE_DIR}/graphics2/GraphicsUtility.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/MeshRendererSubsystem.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/InstanceCache.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/TransformCache.cpp
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

### PostProcess Tests ###
add_executable(PostProcess_tests
    PostProcess_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/GraphicsUtility.cpp
    ${NC_SOURCE_DIR}/graphics2/PostProcess.cpp
)

target_include_directories(PostProcess_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(PostProcess_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(PostProcess_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(PostProcess_tests PostProcess_tests)

### TransformCache Tests ###
add_executable(TransformCache_tests
    TransformCache_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/TransformCache.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
)

target_include_directories(TransformCache_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
)

target_compile_options(TransformCache_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(TransformCache_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(TransformCache_tests TransformCache_tests)
