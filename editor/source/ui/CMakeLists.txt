target_sources(${EDITOR_EXE}
    PRIVATE
        ${ROOT_DIR}/source/ui/EditorUI.cpp
        ${ROOT_DIR}/source/ui/UtilitiesPanel.cpp
)

add_subdirectory(dialog)
add_subdirectory(scene)