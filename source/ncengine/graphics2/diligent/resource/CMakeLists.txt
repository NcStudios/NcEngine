target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GlobalEnvironmentResource.cpp
        GlobalMeshBuffer.cpp
        GlobalResourceSignature.cpp
        GlobalTextureBufferResource.cpp
        MaterialDataBufferResource.cpp
        MaterialResourceSignature.cpp
        ShaderBindings.cpp
)

add_subdirectory(component)
