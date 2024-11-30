# Platform-specific values for graphics integration tests
if(WIN32)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/diligent/NativeWindowWin32.cpp)
elseif(UNIX AND NOT APPLE)
    set(GraphicsTestCommonSources ${NC_SOURCE_DIR}/graphics2/diligent/NativeWindowLinux.cpp)
endif()

set(NC_INTEGRATION_TEST_COLLATERAL_DIR "${CMAKE_CURRENT_LIST_DIR}/collateral")

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

### MeshBuffer Tests ###
add_graphics_test(MeshBuffer_tests)

target_sources(MeshBuffer_tests
    PRIVATE
        MeshBuffer_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/MeshBuffer.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

target_compile_definitions(MeshBuffer_tests
    PRIVATE
        -DNC_RUNTIME_SHADER_COMPILATION
)

### EnvironmentBufferResource Tests ###
add_graphics_test(EnvironmentBufferResource_tests)

target_sources(EnvironmentBufferResource_tests
    PRIVATE
        EnvironmentBufferResource_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/EnvironmentBufferResource.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/ResourceTypes.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/base/UniformBuffer.cpp
)

### TextureBufferResource Tests ###
add_graphics_test(TextureBufferResource_tests)

target_sources(TextureBufferResource_tests
    PRIVATE
        TextureBufferResource_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/TextureBufferResource.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/ResourceTypes.cpp
)

### ShaderFactory Tests ###
add_graphics_test(ShaderFactory_tests)

target_sources(ShaderFactory_tests
    PRIVATE
        ShaderFactory_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/ShaderFactory.cpp
)

target_compile_definitions(ShaderFactory_tests
    PRIVATE
        -DNC_INTEGRATION_TEST_COLLATERAL_DIR="${NC_INTEGRATION_TEST_COLLATERAL_DIR}"
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

target_compile_definitions(ShaderFactory_runtimeCompilation_tests
    PRIVATE
        -DNC_INTEGRATION_TEST_COLLATERAL_DIR="${NC_INTEGRATION_TEST_COLLATERAL_DIR}"
)

### StructuredBuffer Tests ###
add_graphics_test(StructuredBuffer_tests)

target_sources(StructuredBuffer_tests
    PRIVATE
        StructuredBuffer_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/EnvironmentBufferResource.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/base/StructuredBuffer.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/base/UniformBuffer.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/ResourceTypes.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/PerFrameResourceSignature.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/TextureBufferResource.cpp
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

### UniformBuffer Tests ###
add_graphics_test(UniformBuffer_tests)

target_sources(UniformBuffer_tests
    PRIVATE
        UniformBuffer_tests.cpp
        ${NC_SOURCE_DIR}/graphics2/diligent/resource/base/UniformBuffer.cpp
)
