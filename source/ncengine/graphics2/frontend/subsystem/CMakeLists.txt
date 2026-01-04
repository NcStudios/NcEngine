target_sources(${NC_ENGINE_LIB}
    PRIVATE
        CameraSubsystem.cpp
        CascadedShadowMap.cpp
        LightSubsystem.cpp
        MaterialRegistry.cpp
        MeshSubsystem.cpp
        PostProcessSubsystem.cpp
        TransformCache.cpp
        UISubsystem.cpp
        WireframeRendererSubsystem.cpp
)

add_subdirectory(animation)
add_subdirectory(particle)
