### Config Tests ###
add_executable(AudioSource_tests
    AudioSource_tests.cpp
    ${NC_SOURCE_DIR}/audio/AudioSource.cpp
)

target_include_directories(AudioSource_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
)

target_compile_options(AudioSource_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(AudioSource_tests
    PRIVATE
        NcUtility
        NcMath
        gtest_main
)

add_test(AudioSource_tests AudioSource_tests)
