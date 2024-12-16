target_sources(${NC_ENGINE_LIB}
    PRIVATE
        EnvironmentBufferResource.cpp
        MeshBuffer.cpp
        PerFrameResourceSignature.cpp
        PerPassResourceSignature.cpp
        PostProcessPropertyBufferResource.cpp
        PerPassInstanceBufferResource.cpp
        PostProcessColorSinkBufferResource.cpp
        PostProcessDepthSinkBufferResource.cpp
        TextureBufferResource.cpp
        ResourceTypes.cpp
        ShaderBindings.cpp
        WireframeBufferResource.cpp
)

add_subdirectory(base)
