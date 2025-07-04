find_package(Vulkan REQUIRED)

set(NC_TEST_COLLATERAL_DIRECTORY ${PROJECT_SOURCE_DIR}/test/ncengine/asset/collateral/)

### AssetUtilities Tests ###
add_executable(AssetUtilities_tests
    AssetUtilities_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/AssetUtilities.cpp
)

target_include_directories(AssetUtilities_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(AssetUtilities_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(AssetUtilities_tests
    PRIVATE
        NcUtility
        gtest_main
)

add_test(AssetUtilities_tests AssetUtilities_tests)

### AudioClipAssetManager Tests ###
add_executable(AudioClipAssetManager_tests
    AudioClipAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/AudioClipAssetManager.cpp
)

target_compile_definitions(AudioClipAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(AudioClipAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(AudioClipAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(AudioClipAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(AudioClipAssetManager_tests AudioClipAssetManager_tests)

### ConvexHullAssetManager Tests ###
add_executable(ConvexHullAssetManager_tests
    ConvexHullAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/ConvexHullAssetManager.cpp
)

target_compile_definitions(ConvexHullAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConvexHullAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(ConvexHullAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ConvexHullAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(ConvexHullAssetManager_tests ConvexHullAssetManager_tests)

### CubeMapAssetManager Tests ###
add_executable(CubeMapAssetManager_tests
    CubeMapAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/AssetData.cpp
    ${NC_SOURCE_DIR}/asset/manager/AssetUtilities.cpp
    ${NC_SOURCE_DIR}/asset/manager/CubeMapAssetManager.cpp
)

target_compile_definitions(CubeMapAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(CubeMapAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(CubeMapAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(CubeMapAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(CubeMapAssetManager_tests CubeMapAssetManager_tests)

### FontAssetManager Tests ###
add_executable(FontAssetManager_tests
    FontAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/FontAssetManager.cpp
)

target_compile_definitions(FontAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
        NC_TEST_STRIP_DEPENDENCIES
)

target_include_directories(FontAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(FontAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(FontAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        imgui
        gtest_main
)

add_test(FontAssetManager_tests FontAssetManager_tests)

### MeshAssetManager Tests ###
add_executable(MeshAssetManager_tests
    MeshAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/MeshAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/AssetUtilities.cpp
)

target_compile_definitions(MeshAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(MeshAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(MeshAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(MeshAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(MeshAssetManager_tests MeshAssetManager_tests)

### MeshColliderAssetManager Tests ###
add_executable(MeshColliderAssetManager_tests
    MeshColliderAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/MeshColliderAssetManager.cpp
)

target_compile_definitions(MeshColliderAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(MeshColliderAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
)

target_compile_options(MeshColliderAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(MeshColliderAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(MeshColliderAssetManager_tests MeshColliderAssetManager_tests)

### ShaderAssetManager Tests ###
add_executable(ShaderAssetManager_tests
    ShaderAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/ShaderAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/AssetUtilities.cpp
)

target_compile_definitions(ShaderAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ShaderAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ShaderAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ShaderAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(ShaderAssetManager_tests ShaderAssetManager_tests)

### TextureAssetManager Tests ###
add_executable(TextureAssetManager_tests
    TextureAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/TextureAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/AssetData.cpp
)

target_compile_definitions(TextureAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(TextureAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(TextureAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(TextureAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(TextureAssetManager_tests TextureAssetManager_tests)

### ShapeKeyAnimationAssetManager Tests ###
add_executable(ShapeKeyAnimationAssetManager_tests
    ShapeKeyAnimationAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/ShapeKeyAnimationAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/AssetData.cpp
)

target_compile_definitions(ShapeKeyAnimationAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${PROJECT_SOURCE_DIR}/sample/assets/nca/shapekey_animation/"
)

target_include_directories(ShapeKeyAnimationAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(ShapeKeyAnimationAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(ShapeKeyAnimationAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(ShapeKeyAnimationAssetManager_tests ShapeKeyAnimationAssetManager_tests)

### SkeletalAnimationAssetManager Tests ###
add_executable(SkeletalAnimationAssetManager_tests
    SkeletalAnimationAssetManager_tests.cpp
    ${NC_SOURCE_DIR}/asset/manager/SkeletalAnimationAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/AssetData.cpp
)

target_compile_definitions(SkeletalAnimationAssetManager_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(SkeletalAnimationAssetManager_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(SkeletalAnimationAssetManager_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SkeletalAnimationAssetManager_tests
    PRIVATE
        NcAsset
        NcUtility
        gtest_main
)

add_test(SkeletalAnimationAssetManager_tests SkeletalAnimationAssetManager_tests)

### NcAsset Tests ###
add_executable(NcAsset_tests
    NcAsset_tests.cpp
    ${NC_SOURCE_DIR}/asset/NcAssetImpl.cpp
    ${NC_SOURCE_DIR}/asset/AssetData.cpp
    ${NC_SOURCE_DIR}/asset/manager/AssetUtilities.cpp
    ${NC_SOURCE_DIR}/asset/manager/AudioClipAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/ConvexHullAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/CubeMapAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/FontAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/MeshAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/MeshColliderAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/ShaderAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/ShapeKeyAnimationAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/SkeletalAnimationAssetManager.cpp
    ${NC_SOURCE_DIR}/asset/manager/TextureAssetManager.cpp
)

target_compile_definitions(NcAsset_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
        NC_TEST_STRIP_DEPENDENCIES
)

target_include_directories(NcAsset_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(NcAsset_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(NcAsset_tests
    PRIVATE
        NcAsset
        NcUtility
        imgui
        gtest_main
)

add_test(NcAsset_tests NcAsset_tests)
