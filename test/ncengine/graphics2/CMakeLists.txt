### CameraSubsystem Tests ###
add_executable(CameraSubsystem_tests
    CameraSubsystem_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/GraphicsUtilities.cpp
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

### MeshRendererSubsystem Tests ###
add_executable(MeshRendererSubsystem_tests
    MeshRendererSubsystem_tests.cpp
    ${NC_SOURCE_DIR}/ecs/Transform.cpp
    ${NC_SOURCE_DIR}/graphics/components/ToonRenderer.cpp
    ${NC_SOURCE_DIR}/graphics2/frontend/GraphicsUtilities.cpp
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

