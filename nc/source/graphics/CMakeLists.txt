target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/graphics/Base.cpp
        ${ROOT_DIR}/source/graphics/Commands.cpp
        ${ROOT_DIR}/source/graphics/Graphics.cpp
        ${ROOT_DIR}/source/graphics/Initializers.cpp
        ${ROOT_DIR}/source/graphics/MeshManager.cpp
        ${ROOT_DIR}/source/graphics/PerFrameRenderState.cpp
        ${ROOT_DIR}/source/graphics/Renderer.cpp
        ${ROOT_DIR}/source/graphics/ShaderUtilities.cpp
        ${ROOT_DIR}/source/graphics/Swapchain.cpp
        ${ROOT_DIR}/source/graphics/TextureManager.cpp
        ${ROOT_DIR}/source/graphics/vma.cpp
)

add_subdirectory(resources)
add_subdirectory(techniques)
