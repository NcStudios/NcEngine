if (WIN32)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            NativeWindowWin32.cpp
    )
elseif (UNIX AND NOT APPLE)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            NativeWindowLinux.cpp
    )
endif()

target_sources(${NC_ENGINE_LIB}
    PRIVATE
        DiligentEngine.cpp
        ImGuiImplGLFW.cpp
        ShaderFactory.cpp
        UIBackend.cpp
)

add_subdirectory(pass)
add_subdirectory(resource)
