if(USE_VULKAN)
    target_sources(${GAME_EXE}
        PRIVATE 
            ${ROOT_DIR}/source/jare_scratch/VulkanScene.cpp
    )
endif()