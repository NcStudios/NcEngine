set(NC_CONVERT_TEST_COLLATERAL_DIR ${CMAKE_CURRENT_LIST_DIR}/collateral)

### Asset Serialization Tests ###
add_executable(AssetSerialization_integration_tests
    AssetSerialization_integration_tests.cpp
)

target_compile_options(AssetSerialization_integration_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(AssetSerialization_integration_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncasset
        ${PROJECT_SOURCE_DIR}/source/common
        ${PROJECT_SOURCE_DIR}/source/ncconvert
        ${PROJECT_SOURCE_DIR}/source/ncconvert/builder
        ${PROJECT_SOURCE_DIR}/source/ncconvert/utility
)

target_sources(AssetSerialization_integration_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncasset/Deserialize.cpp
        ${PROJECT_SOURCE_DIR}/source/ncasset/NcaHeader.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/builder/Serialize.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/BlobSize.cpp
)

target_link_libraries(AssetSerialization_integration_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(AssetSerialization_integration_tests AssetSerialization_integration_tests)

## AudioConverter Tests ###
add_executable(AudioConverter_unit_tests
    AudioConverter_unit_tests.cpp
)

target_compile_options(AudioConverter_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_compile_definitions(AudioConverter_unit_tests
    PRIVATE
        TEST_COLLATERAL_DIR="${NC_CONVERT_TEST_COLLATERAL_DIR}"
)

target_include_directories(AudioConverter_unit_tests
    PRIVATE
        ${NC_CONVERT_TEST_COLLATERAL_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_include_directories(AudioConverter_unit_tests
    SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}/source/external
)

target_sources(AudioConverter_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/AudioConverter.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/Path.cpp
)

target_link_libraries(AudioConverter_unit_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(AudioConverter_unit_tests AudioConverter_unit_tests)

## Build and Import Tests ###
if(NC_BUILD_NCCONVERT)
    add_executable(BuildAndImport_integration_tests
        BuildAndImport_integration_tests.cpp
    )

    target_compile_options(BuildAndImport_integration_tests
        PUBLIC
            ${NC_COMPILER_FLAGS}
    )

    target_compile_definitions(BuildAndImport_integration_tests
        PRIVATE
            TEST_COLLATERAL_DIR="${NC_CONVERT_TEST_COLLATERAL_DIR}"
    )

    target_include_directories(BuildAndImport_integration_tests
        PRIVATE
            ${NC_CONVERT_TEST_COLLATERAL_DIR}
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/include/ncasset
            ${PROJECT_SOURCE_DIR}/source
            ${PROJECT_SOURCE_DIR}/source/common
            ${PROJECT_SOURCE_DIR}/source/ncconvert
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis
            ${PROJECT_SOURCE_DIR}/source/ncconvert/builder
            ${PROJECT_SOURCE_DIR}/source/ncconvert/converters
            ${PROJECT_SOURCE_DIR}/source/ncconvert/utility
    )

    target_include_directories(BuildAndImport_integration_tests
        SYSTEM PRIVATE
            ${PROJECT_SOURCE_DIR}/source/external
    )

    target_sources(BuildAndImport_integration_tests
        PRIVATE
            ${PROJECT_SOURCE_DIR}/source/ncasset/Deserialize.cpp
            ${PROJECT_SOURCE_DIR}/source/ncasset/Import.cpp
            ${PROJECT_SOURCE_DIR}/source/ncasset/NcaHeader.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/GeometryAnalysis.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/Sanitize.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/TextureAnalysis.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/builder/Builder.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/builder/Serialize.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/AudioConverter.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/GeometryConverter.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/TextureConverter.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/BlobSize.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/EnumExtensions.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/Path.cpp
    )

    target_link_libraries(BuildAndImport_integration_tests
        PRIVATE
            gtest_main
            NcMath
            assimp::assimp
    )

    add_test(BuildAndImport_integration_tests BuildAndImport_integration_tests)
endif()

## EnumExtensions Tests ###
add_executable(EnumExtensions_unit_tests
    EnumExtensions_unit_tests.cpp
)

target_compile_options(EnumExtensions_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(EnumExtensions_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_sources(EnumExtensions_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/EnumExtensions.cpp
)

target_link_libraries(EnumExtensions_unit_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(EnumExtensions_unit_tests EnumExtensions_unit_tests)

## GeometryConverter Tests ###
if(NC_BUILD_NCCONVERT)
    add_executable(GeometryConverter_unit_tests
        GeometryConverter_unit_tests.cpp
    )

    target_compile_options(GeometryConverter_unit_tests
        PUBLIC
            ${NC_COMPILER_FLAGS}
    )

    target_compile_definitions(GeometryConverter_unit_tests
        PRIVATE
            TEST_COLLATERAL_DIR="${NC_CONVERT_TEST_COLLATERAL_DIR}"
    )

    target_include_directories(GeometryConverter_unit_tests
        PRIVATE
            ${NC_CONVERT_TEST_COLLATERAL_DIR}
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/source/ncconvert
    )

    target_sources(GeometryConverter_unit_tests
        PRIVATE
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/GeometryAnalysis.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/Sanitize.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/GeometryConverter.cpp
            ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/Path.cpp
    )

    target_link_libraries(GeometryConverter_unit_tests
        PRIVATE
            gtest_main
            NcMath
            assimp::assimp
    )

    add_test(GeometryConverter_unit_tests GeometryConverter_unit_tests)
endif()

## GeometryAnalysis Tests ###
add_executable(GeometryAnalysis_unit_tests
    GeometryAnalysis_tests.cpp
)

target_compile_options(GeometryAnalysis_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(GeometryAnalysis_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_sources(GeometryAnalysis_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/GeometryAnalysis.cpp
)

target_link_libraries(GeometryAnalysis_unit_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(GeometryAnalysis_unit_tests GeometryAnalysis_unit_tests)

## TextureAnalysis Tests ###
add_executable(TextureAnalysis_unit_tests
    TextureAnalysis_unit_tests.cpp
)

target_compile_options(TextureAnalysis_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_include_directories(TextureAnalysis_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_sources(TextureAnalysis_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/TextureAnalysis.cpp
)

target_link_libraries(TextureAnalysis_unit_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(TextureAnalysis_unit_tests TextureAnalysis_unit_tests)

## TextureConverter Tests ###
add_executable(TextureConverter_unit_tests
    TextureConverter_unit_tests.cpp
)

target_compile_options(TextureConverter_unit_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_compile_definitions(TextureConverter_unit_tests
    PRIVATE
        TEST_COLLATERAL_DIR="${NC_CONVERT_TEST_COLLATERAL_DIR}"
)

target_include_directories(TextureConverter_unit_tests
    PRIVATE
        ${NC_CONVERT_TEST_COLLATERAL_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_include_directories(TextureConverter_unit_tests
    SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}/source/external
)

target_sources(TextureConverter_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/analysis/TextureAnalysis.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/TextureConverter.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/utility/Path.cpp
)

target_link_libraries(TextureConverter_unit_tests
    PRIVATE
        gtest_main
        NcMath
)

add_test(TextureConverter_unit_tests TextureConverter_unit_tests)

## NcConvert Integration Tests ##
if(NC_BUILD_NCCONVERT)
    add_executable(NcConvert_integration_tests
        NcConvert_integration_tests.cpp
    )

    add_dependencies(NcConvert_integration_tests nc-convert)

    target_compile_options(NcConvert_integration_tests
        PUBLIC
            ${NC_COMPILER_FLAGS}
    )

    target_compile_definitions(NcConvert_integration_tests
        PRIVATE
            TEST_COLLATERAL_DIR="${NC_CONVERT_TEST_COLLATERAL_DIR}"
            NC_CONVERT_EXECUTABLE_PATH="$<TARGET_FILE:nc-convert>"
    )

    target_include_directories(NcConvert_integration_tests
        PRIVATE
            ${NC_CONVERT_TEST_COLLATERAL_DIR}
            ${PROJECT_SOURCE_DIR}/source/ncconvert
    )

    target_include_directories(NcConvert_integration_tests
        SYSTEM PRIVATE
            ${PROJECT_SOURCE_DIR}/source/external
    )

    target_link_libraries(NcConvert_integration_tests
        PRIVATE
            gtest_main
            NcUtility
    )

    add_test(NcConvert_integration_tests NcConvert_integration_tests)
endif()
