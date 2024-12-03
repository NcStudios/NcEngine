target_sources(${NC_ENGINE_LIB}
    PRIVATE
        EnvironmentBufferResource.cpp
        MeshBuffer.cpp
        PerFrameResourceSignature.cpp
        PerPassResourceSignature.cpp
        PostProcessPropertyBufferResource.cpp
        PostProcessSinkBufferResource.cpp
        TextureBufferResource.cpp
        ResourceTypes.cpp
        ShaderBindings.cpp
)

add_subdirectory(base)
