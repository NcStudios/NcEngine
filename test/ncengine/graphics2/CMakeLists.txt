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
