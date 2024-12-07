target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GraphicsUtility.cpp
        Material.cpp
        MeshRenderer2.cpp
        NcGraphicsImpl2.cpp
        PostProcess.cpp
        SkeletalAnimationController.cpp
        SkinnedMesh.cpp
)

add_subdirectory(diligent)
add_subdirectory(frontend)
