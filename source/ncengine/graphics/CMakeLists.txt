target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GraphicsUtilities.cpp
        IGraphics.cpp
        NcGraphicsImpl.cpp
)

add_subdirectory(api/vulkan)
add_subdirectory(components)
add_subdirectory(shader_resource)
add_subdirectory(system)
