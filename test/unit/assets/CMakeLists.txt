find_package(Vulkan REQUIRED)

set(NC_TEST_COLLATERAL_DIRECTORY ${PROJECT_SOURCE_DIR}/unit/assets/collateral/)

### ConcaveColliderAssetManager Tests ###
add_executable(ConcaveColliderAssetManager_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/assets/ConcaveColliderAssetManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/assets/ConcaveColliderAssetManager.cpp
)

target_compile_definitions(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ConcaveColliderAssetManager_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ConcaveColliderAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(ConcaveColliderAssetManager_unit_tests ConcaveColliderAssetManager_unit_tests)

### ConvexHullAssetManager Tests ###
add_executable(ConvexHullAssetManager_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/assets/ConvexHullAssetManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/assets/ConvexHullAssetManager.cpp
)

target_compile_definitions(ConvexHullAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(ConvexHullAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(ConvexHullAssetManager_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(ConvexHullAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(ConvexHullAssetManager_unit_tests ConvexHullAssetManager_unit_tests)

### MeshAssetManager Tests ###
add_executable(MeshAssetManager_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/assets/MeshAssetManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/assets/MeshAssetManager.cpp
)

target_compile_definitions(MeshAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(MeshAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(MeshAssetManager_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(MeshAssetManager_unit_tests
    PRIVATE
        gtest
        Vulkan::Vulkan
)

add_test(MeshAssetManager_unit_tests MeshAssetManager_unit_tests)

### AudioClipAssetManager Tests ###
add_executable(AudioClipAssetManager_unit_tests
    ${PROJECT_SOURCE_DIR}/unit/assets/AudioClipAssetManager_unit_tests.cpp
    ${NC_SOURCE_DIR}/assets/AudioClipAssetManager.cpp
)

target_compile_definitions(AudioClipAssetManager_unit_tests
    PRIVATE
        NC_TEST_COLLATERAL_DIRECTORY="${NC_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(AudioClipAssetManager_unit_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}/gtest/googletest/include
)

target_compile_options(AudioClipAssetManager_unit_tests
    PUBLIC
        ${WARNING_FLAGS}
)

target_link_libraries(AudioClipAssetManager_unit_tests
    PRIVATE
        gtest
)

add_test(AudioClipAssetManager_unit_tests AudioClipAssetManager_unit_tests)