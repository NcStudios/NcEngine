target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/ecs/ColliderSystem.cpp
        ${ROOT_DIR}/source/ecs/ColliderTree.cpp
        ${ROOT_DIR}/source/ecs/EntityComponentSystem.cpp
        ${ROOT_DIR}/source/ecs/EcsApiImpl.cpp
        ${ROOT_DIR}/source/ecs/ParticleEmitterSystem.cpp
        ${ROOT_DIR}/source/ecs/SoA.cpp
)

if(USE_VULKAN)
    target_sources(${ENGINE_LIB}
        PRIVATE 
            ${ROOT_DIR}/source/ecs/MeshRendererSystem.cpp
            ${ROOT_DIR}/source/ecs/PointLightSystem.cpp
    )
endif()