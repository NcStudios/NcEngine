# Platform-specific values for graphics integration tests
if(WIN32)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/diligent/NativeWindowWin32.cpp)
elseif(UNIX AND NOT APPLE)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/diligent/NativeWindowLinux.cpp)
endif()

# Common setup for graphics integration tests
function(add_graphics_test target)
    add_executable(${target}
        ${GraphicsTestCommonSources}
        ${NC_SOURCE_DIR}/graphics2/diligent/DiligentEngine.cpp
    )

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

### DiligentEngine Tests ###
add_graphics_test(DiligentEngine_tests)

target_sources(DiligentEngine_tests
    PRIVATE
        DiligentEngine_tests.cpp
)

### GlobalMeshBuffer Tests ###
add_graphics_test(GlobalMeshBuffer_tests)

target_sources(GlobalMeshBuffer_tests
    PRIVATE
        GlobalMeshBuffer_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/GlobalMeshBuffer.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

target_compile_definitions(GlobalMeshBuffer_tests
    PRIVATE
        -DNC_RUNTIME_SHADER_COMPILATION
)

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

### StructuredBuffer Tests ###
add_graphics_test(StructuredBuffer_tests)

target_sources(StructuredBuffer_tests
    PRIVATE
        StructuredBuffer_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/base/StructuredBuffer.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/MaterialResourceSignature.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

target_compile_definitions(StructuredBuffer_tests
    PRIVATE
        -DNC_RUNTIME_SHADER_COMPILATION
)

### UIBackend Tests ###
add_graphics_test(UIBackend_tests)

target_sources(UIBackend_tests
    PRIVATE
    UIBackend_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/UIBackend.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ImGuiImplGLFW.cpp
        ${NC_SOURCE_DIR}/graphics2/frontend/subsystem/UISubsystem.cpp
        ${NC_SOURCE_DIR}/ui/editor/stub/EditorStub.cpp
)

target_link_libraries(UIBackend_tests
    PRIVATE
        imgui
)
