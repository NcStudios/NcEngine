target_sources(${EDITOR_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/EditorConfig.cpp
        ${PROJECT_SOURCE_DIR}/source/EditorScene.cpp
        ${PROJECT_SOURCE_DIR}/source/main.cpp
)

add_subdirectory(assets)
add_subdirectory(framework)
add_subdirectory(serialize)
add_subdirectory(ui)
add_subdirectory(utility)