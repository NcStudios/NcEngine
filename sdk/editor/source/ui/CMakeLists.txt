target_sources(${EDITOR_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ui/EditorUI.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/EnvironmentPanel.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/UtilitiesPanel.cpp
)

add_subdirectory(dialog)
add_subdirectory(scene)