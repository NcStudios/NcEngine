### DiligentEngineWin32Int_tests Tests ###
add_executable(DiligentEngineWin32Int_tests
    DiligentEngineWin32Int_tests.cpp
        ${NC_SOURCE_DIR}/config/Config.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/DiligentEngineWin32.cpp
        ${NC_SOURCE_DIR}/graphics2/NcGraphicsImpl2Win32.cpp
)

target_include_directories(DiligentEngineWin32Int_tests
    PRIVATE
        ${NC_INCLUDE_DIR}
        ${NC_INCLUDE_DIR}/ncengine
        ${NC_SOURCE_DIR}
        ${NC_EXTERNAL_DIR}
        ${diligentcore_SOURCE_DIR}
)

target_compile_options(DiligentEngineWin32Int_tests
    PUBLIC
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(DiligentEngineWin32Int_tests
    PRIVATE
        gtest_main
        glfw
        NcMath
        ${DILIGENT_LIBRARIES}
)

# Diligent helper method to get required DLLs and EXEs into the project's executable dir.
copy_required_dlls(DiligentEngineWin32Int_tests)

add_test(DiligentEngineWin32Int_tests DiligentEngineWin32Int_tests)
