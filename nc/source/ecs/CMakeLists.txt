target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/ecs/Entity.cpp
        ${ROOT_DIR}/source/ecs/EntityComponentSystem.cpp
        ${ROOT_DIR}/source/ecs/EcsApiImpl.cpp
        ${ROOT_DIR}/source/ecs/SoA.cpp
)

if(USE_VULKAN)
    target_sources(${ENGINE_LIB}
        PRIVATE 
            ${ROOT_DIR}/source/ecs/RendererSystem.cpp
    )
endif()