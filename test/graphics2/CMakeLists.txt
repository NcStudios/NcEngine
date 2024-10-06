### DiligentEngine_tests Tests ###
add_executable(DiligentEngine_tests
DiligentEngine_tests.cpp
    ${NC_SOURCE_DIR}/graphics2/DiligentEngine.cpp
)

target_include_directories(DiligentEngine_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
)

target_compile_options(DiligentEngine_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(DiligentEngine_tests
    PRIVATE
        gtest_main
        NcMath
)

# Diligent helper method to get required DLLs and EXEs into the project's executable dir.
if(WIN32)
    copy_required_dlls(DiligentEngine_tests)
endif()

add_test(DiligentEngine_tests DiligentEngine_tests)
