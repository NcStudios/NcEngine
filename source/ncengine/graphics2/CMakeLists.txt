target_sources(${NC_ENGINE_LIB}
    PRIVATE
        Camera.cpp
        GraphicsUtility.cpp
        Material.cpp
        Mesh.cpp
        NcGraphicsImpl2.cpp
        ParticleEmitter.cpp
        PostProcess.cpp
        SceneNavigationCamera.cpp
        SkeletalAnimationController.cpp
)

add_subdirectory(diligent)
add_subdirectory(frontend)
