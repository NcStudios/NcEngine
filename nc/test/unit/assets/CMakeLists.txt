find_package(Vulkan REQUIRED)

set(NC_TEST_COLLATERAL_DIRECTORY ${ROOT_DIR}/test/unit/assets/collateral)

### ConcaveColliderAssetManager Tests ###
add_executable(ConcaveColliderAssetManager_unit_tests
    ${ROOT_DIR}/test/unit/assets/ConcaveColliderAssetManager_unit_tests.cpp
    ${ROOT_DIR}/source/assets/ConcaveColliderAssetManager.cpp
)

target_compile_definitions(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(ConcaveColliderAssetManager_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(ConcaveColliderAssetManager_unit_tests ConcaveColliderAssetManager_unit_tests)

### ConvexHullAssetManager Tests ###
add_executable(ConvexHullAssetManager_unit_tests
    ${ROOT_DIR}/test/unit/assets/ConvexHullAssetManager_unit_tests.cpp
    ${ROOT_DIR}/source/assets/ConvexHullAssetManager.cpp
)

target_compile_definitions(ConvexHullAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConvexHullAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(ConvexHullAssetManager_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(ConvexHullAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(ConvexHullAssetManager_unit_tests ConvexHullAssetManager_unit_tests)

### MeshAssetManager Tests ###
add_executable(MeshAssetManager_unit_tests
    ${ROOT_DIR}/test/unit/assets/MeshAssetManager_unit_tests.cpp
    ${ROOT_DIR}/source/assets/MeshAssetManager.cpp
)

target_compile_definitions(MeshAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(MeshAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/include
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(MeshAssetManager_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(MeshAssetManager_unit_tests
    PRIVATE
        gtest
        Vulkan::Vulkan
)

add_test(MeshAssetManager_unit_tests MeshAssetManager_unit_tests)

### SoundClipAssetManager Tests ###
add_executable(SoundClipAssetManager_unit_tests
    ${ROOT_DIR}/test/unit/assets/SoundClipAssetManager_unit_tests.cpp
    ${ROOT_DIR}/source/assets/SoundClipAssetManager.cpp
)

target_compile_definitions(SoundClipAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(SoundClipAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${ROOT_DIR}/external/gtest/googletest/include
)

target_compile_options(SoundClipAssetManager_unit_tests
    PUBLIC
        ${GCC_WARNING_FLAGS}
)

target_link_libraries(SoundClipAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(SoundClipAssetManager_unit_tests SoundClipAssetManager_unit_tests)