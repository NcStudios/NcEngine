find_package(Vulkan REQUIRED)

set(NC_TEST_COLLATERAL_DIRECTORY ${PROJECT_SOURCE_DIR}/test/assets/collateral/)

### AssetId Tests ###
add_executable(AssetId_tests
    AssetId_tests.cpp
)

target_include_directories(AssetId_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(AssetId_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(AssetId_tests
    PRIVATE
        gtest
)

add_test(AssetId_tests AssetId_tests)

### AudioClipAssetManager Tests ###
add_executable(AudioClipAssetManager_tests
    AudioClipAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/AudioClipAssetManager.cpp
)

target_compile_definitions(AudioClipAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(AudioClipAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(AudioClipAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(AudioClipAssetManager_tests
    PRIVATE
        gtest
)

add_test(AudioClipAssetManager_tests AudioClipAssetManager_tests)

### ConcaveColliderAssetManager Tests ###
add_executable(ConcaveColliderAssetManager_tests
    ConcaveColliderAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/ConcaveColliderAssetManager.cpp
)

target_compile_definitions(ConcaveColliderAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConcaveColliderAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ConcaveColliderAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ConcaveColliderAssetManager_tests
    PRIVATE
        gtest
)

add_test(ConcaveColliderAssetManager_tests ConcaveColliderAssetManager_tests)

### ConvexHullAssetManager Tests ###
add_executable(ConvexHullAssetManager_tests
    ConvexHullAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/ConvexHullAssetManager.cpp
)

target_compile_definitions(ConvexHullAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConvexHullAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ConvexHullAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ConvexHullAssetManager_tests
    PRIVATE
        gtest
)

add_test(ConvexHullAssetManager_tests ConvexHullAssetManager_tests)

### CubeMapAssetManager Tests ###
add_executable(CubeMapAssetManager_tests
    CubeMapAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/AssetData.cpp
    ${NC_SOURCE_DIR}/assets/AssetUtilities.cpp
    ${NC_SOURCE_DIR}/assets/CubeMapAssetManager.cpp
)

target_compile_definitions(CubeMapAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(CubeMapAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(CubeMapAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(CubeMapAssetManager_tests
    PRIVATE
        gtest
)

add_test(CubeMapAssetManager_tests CubeMapAssetManager_tests)

### MeshAssetManager Tests ###
add_executable(MeshAssetManager_tests
    MeshAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/MeshAssetManager.cpp
    ${NC_SOURCE_DIR}/assets/AssetUtilities.cpp
)

target_compile_definitions(MeshAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(MeshAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(MeshAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(MeshAssetManager_tests
    PRIVATE
        gtest
)

add_test(MeshAssetManager_tests MeshAssetManager_tests)

### ShaderAssetManager Tests ###
add_executable(ShaderAssetManager_tests
    ShaderAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/ShaderAssetManager.cpp
    ${NC_SOURCE_DIR}/assets/AssetUtilities.cpp
)

target_compile_definitions(ShaderAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ShaderAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ShaderAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ShaderAssetManager_tests
    PRIVATE
        gtest
)

add_test(ShaderAssetManager_tests ShaderAssetManager_tests)

### TextureAssetManager Tests ###
add_executable(TextureAssetManager_tests
    TextureAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/assets/TextureAssetManager.cpp
    ${NC_SOURCE_DIR}/assets/AssetData.cpp
)

target_compile_definitions(TextureAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(TextureAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(TextureAssetManager_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(TextureAssetManager_tests
    PRIVATE
        gtest
)

add_test(TextureAssetManager_tests TextureAssetManager_tests)