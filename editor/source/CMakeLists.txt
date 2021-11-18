target_sources(${EDITOR_EXE}
    PRIVATE
        ${ROOT_DIR}/source/EditorConfig.cpp
        ${ROOT_DIR}/source/EditorScene.cpp
        ${ROOT_DIR}/source/main.cpp
)

add_subdirectory(assets)
add_subdirectory(framework)
add_subdirectory(serialize)
add_subdirectory(ui)
add_subdirectory(utility)