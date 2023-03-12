target_sources(${NC_ENGINE_LIB}
    PRIVATE
        IGraphics.cpp
        MeshBatcher.cpp
        NcGraphicsImpl.cpp
        PerFrameRenderState.cpp
)

add_subdirectory(api/vulkan)
add_subdirectory(components)
add_subdirectory(debug)
add_subdirectory(system)
