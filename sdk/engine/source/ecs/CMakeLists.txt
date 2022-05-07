target_sources(NcEngineLib
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ecs/ParticleEmitterSystem.cpp
        ${PROJECT_SOURCE_DIR}/source/ecs/PointLightSystem.cpp
        ${PROJECT_SOURCE_DIR}/source/ecs/Registry.cpp
        ${PROJECT_SOURCE_DIR}/source/ecs/SoA.cpp
)

add_subdirectory(component)