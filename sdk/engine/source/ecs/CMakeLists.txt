target_sources(${NC_ENGINE_LIB}
    PRIVATE
        LogicModule.cpp
        ParticleEmitterSystem.cpp
        PointLightSystem.cpp
        Registry.cpp
        SoA.cpp
)

add_subdirectory(component)