### SkeletalAnimationCalculations_tests Tests ###
add_executable(SkeletalAnimationCalculations_tests
    SkeletalAnimationCalculations_tests.cpp
    ${NC_SOURCE_DIR}/graphics/system/SkeletalAnimationCalculations.cpp
    ${NC_SOURCE_DIR}/graphics/system/SkeletalAnimationTypes.cpp
)

target_include_directories(SkeletalAnimationCalculations_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(SkeletalAnimationCalculations_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(SkeletalAnimationCalculations_tests
    PRIVATE
        gtest_main
        NcAsset
        NcMath
        NcUtility
        Taskflow
)

add_test(SkeletalAnimationCalculations_tests SkeletalAnimationCalculations_tests)

### StateSparseSet Tests ###
add_executable(StableSet_tests
    StableSet_tests.cpp
    ${NC_SOURCE_DIR}/graphics/system/SkeletalAnimationTypes.cpp
)

target_include_directories(StableSet_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(StableSet_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(StableSet_tests
    PRIVATE
        gtest_main
        NcAsset
        NcMath
        NcUtility
        Taskflow
)

add_test(StableSet_tests StableSet_tests)