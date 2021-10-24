target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/ecs/EntityComponentSystem.cpp
        ${ROOT_DIR}/source/ecs/ParticleEmitterSystem.cpp
        ${ROOT_DIR}/source/ecs/PointLightSystem.cpp
        ${ROOT_DIR}/source/ecs/Registry.cpp
        ${ROOT_DIR}/source/ecs/SoA.cpp
)

add_subdirectory(component)