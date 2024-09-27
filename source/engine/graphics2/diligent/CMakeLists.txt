if (PLATFORM_WIN32)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            DiligentEngineWin32.cpp
    )
endif()
if (PLATFORM_LINUX)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            DiligentEngineLinux.cpp
    )
endif()
