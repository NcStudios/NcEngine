target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ComponentWidgets.cpp
        EditorImpl.cpp
        EditorUI.cpp
        SandboxScene.cpp
)

add_subdirectory(assets)
add_subdirectory(windows)
