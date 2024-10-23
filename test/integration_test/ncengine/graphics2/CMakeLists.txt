# Platform-specific values for graphics integration tests
if(WIN32)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/NcGraphicsImpl2Win32.cpp
                                  ${NC_SOURCE_DIR}/graphics2/diligent/DiligentEngineWin32.cpp
    )
elseif(UNIX AND NOT APPLE)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/NcGraphicsImpl2Linux.cpp
                                  ${NC_SOURCE_DIR}/graphics2/diligent/DiligentEngineLinux.cpp
    )
endif()

# Common setup for graphics integration tests
function(add_graphics_test target)
    add_executable(${target} ${GraphicsTestCommonSources})
    add_test(${target} ${target})

    target_include_directories(${target}
        PRIVATE
            ${NC_INCLUDE_DIR}
            ${NC_SOURCE_DIR}
            ${NC_EXTERNAL_DIR}
            ${diligentcore_SOURCE_DIR}
    )

    target_compile_options(${target}
        PUBLIC
            ${NC_COMPILER_FLAGS}
    )

    target_link_libraries(${target}
        PRIVATE
            gtest_main
            glfw
            NcMath
            ${DILIGENT_LIBRARIES}
    )
endfunction()

### DiligentEngineInit_tests Tests ###
add_graphics_test(DiligentEngineInit_tests)

if(WIN32)
    target_sources(DiligentEngineInit_tests
        PRIVATE
            DiligentEngineWin32Int_tests.cpp
    )
elseif(UNIX AND NOT APPLE)
    target_sources(DiligentEngineInit_tests
        PRIVATE
            DiligentEngineLinuxInt_tests.cpp
    )
endif()

### GlobalTextureBufferResource Tests ###
add_graphics_test(GlobalEnvironmentResource_tests)

target_sources(GlobalEnvironmentResource_tests
    PRIVATE
        GlobalEnvironmentResource_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/GlobalEnvironmentResource.cpp
)

### GlobalTextureBufferResource Tests ###
add_graphics_test(GlobalTextureBufferResource_tests)

target_sources(GlobalTextureBufferResource_tests
    PRIVATE
        GlobalTextureBufferResource_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/GlobalTextureBufferResource.cpp
)

### ShaderFactory Tests ###
add_graphics_test(ShaderFactory_tests)

target_sources(ShaderFactory_tests
    PRIVATE
        ShaderFactory_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

### ShaderFactory Runtime Compilation Tests ###
add_graphics_test(ShaderFactory_runtimeCompilation_tests)

target_sources(ShaderFactory_runtimeCompilation_tests
    PRIVATE
        ShaderFactory_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

target_compile_definitions(ShaderFactory_runtimeCompilation_tests
    PRIVATE
        -DNC_RUNTIME_SHADER_COMPILATION
)
