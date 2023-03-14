target_sources(${NC_ENGINE_LIB}
    PRIVATE
        Environment.cpp
        IGraphics.cpp
        MeshBatcher.cpp
        NcGraphicsImpl.cpp
        PerFrameRenderState.cpp
        PointLightSystem.cpp
)

add_subdirectory(api/vulkan)
add_subdirectory(components)
add_subdirectory(debug)
