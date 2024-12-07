target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GraphicsUtility.cpp
        Material.cpp
        NcGraphicsImpl2.cpp
        PostProcess.cpp
        SkeletalAnimationController.cpp
        SkinnedMesh.cpp
        StaticMesh.cpp
)

add_subdirectory(diligent)
add_subdirectory(frontend)
