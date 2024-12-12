target_sources(${NC_ENGINE_LIB}
    PRIVATE
        GraphicsUtility.cpp
        Material.cpp
        Mesh.cpp
        NcGraphicsImpl2.cpp
        PostProcess.cpp
        SkeletalAnimationController.cpp
)

add_subdirectory(diligent)
add_subdirectory(frontend)
