target_sources(${NC_ENGINE_LIB}
    PRIVATE
        CubeMapBufferResource.cpp
        CubeSinkBufferResource.cpp
        EnvironmentBufferResource.cpp
        MeshBuffer.cpp
        PerFrameResourceSignature.cpp
        PerPassResourceSignature.cpp
        PostProcessPropertyBufferResource.cpp
        ResourceTypes.cpp
        ShaderBindings.cpp
        SinkBufferResource.cpp
        SinkIndexBufferResource.cpp
        TextureBufferResource.cpp
        WireframeBufferResource.cpp
)

add_subdirectory(base)
