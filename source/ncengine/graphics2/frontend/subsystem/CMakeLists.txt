target_sources(${NC_ENGINE_LIB}
    PRIVATE
        CameraSubsystem.cpp
        LightSubsystem.cpp
        MaterialRegistry.cpp
        MeshSubsystem.cpp
        ParticleSubsystem.cpp
        PostProcessSubsystem.cpp
        TransformCache.cpp
        UISubsystem.cpp
        WireframeRendererSubsystem.cpp
)

add_subdirectory(animation)
