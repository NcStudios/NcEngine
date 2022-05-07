target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ParticleEmitterSystem.cpp
        PointLightSystem.cpp
        Registry.cpp
        SoA.cpp
)

add_subdirectory(component)