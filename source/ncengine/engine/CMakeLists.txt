target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ComponentFactories.cpp
        ModuleFactory.cpp
        NcEngineImpl.cpp
        RegistryFactory.cpp
)

add_subdirectory(registration)
