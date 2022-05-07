target_sources(${EDITOR_EXE}
    PRIVATE
        EditorUI.cpp
        EnvironmentPanel.cpp
        UtilitiesPanel.cpp
)

add_subdirectory(dialog)
add_subdirectory(scene)