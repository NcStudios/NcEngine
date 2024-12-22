target_sources(${NC_ENGINE_LIB}
    PRIVATE
        EnvironmentBufferResource.cpp
        MeshBuffer.cpp
        PerFrameResourceSignature.cpp
        PerPassResourceSignature.cpp
        PostProcessPropertyBufferResource.cpp
        PostProcessSinkIndexBufferResource.cpp
        ResourceTypes.cpp
        ShaderBindings.cpp
        SinkBufferResource.cpp
        TextureBufferResource.cpp
        WireframeBufferResource.cpp
)

add_subdirectory(base)
