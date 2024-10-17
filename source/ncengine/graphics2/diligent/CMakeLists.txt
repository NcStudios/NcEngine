if (WIN32)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            DiligentEngineWin32.cpp
    )
elseif (UNIX AND NOT APPLE)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            DiligentEngineLinux.cpp
    )
endif()

target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ShaderFactory.cpp
        TestPipeline.cpp
)

add_subdirectory(resource)
