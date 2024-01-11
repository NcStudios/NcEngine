target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GraphicsUtilities.cpp
        IGraphics.cpp
        NcGraphicsImpl.cpp
)

add_subdirectory(api/vulkan)
add_subdirectory(components)
add_subdirectory(debug)
add_subdirectory(system)
