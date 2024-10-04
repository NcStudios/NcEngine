### GetAssetType Tests ###
add_executable(GetAssetType_unit_tests
    GetAssetType_unit_tests.cpp
)

target_compile_options(GetAssetType_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(GetAssetType_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_sources(GetAssetType_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncasset/NcaHeader.cpp
)

target_link_libraries(GetAssetType_unit_tests
    PRIVATE
        gtest_main
        NcUtility
)

add_test(GetAssetType_unit_tests GetAssetType_unit_tests)

### Version Tests ###
add_executable(Version_unit_tests
    GetAssetType_unit_tests.cpp
)

target_compile_options(Version_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(Version_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_sources(Version_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncasset/NcaHeader.cpp
)

target_link_libraries(Version_unit_tests
    PRIVATE
        gtest_main
        NcUtility
)

add_test(Version_unit_tests Version_unit_tests)
