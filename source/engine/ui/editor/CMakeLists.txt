target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ComponentWidgets.cpp
        Editor.cpp
        EditorUI.cpp
)

add_subdirectory(assets)
add_subdirectory(windows)
