target_sources(${EDITOR_EXE}
    PRIVATE
        EditorConfig.cpp
        EditorScene.cpp
        main.cpp
)

add_subdirectory(assets)
add_subdirectory(framework)
add_subdirectory(serialize)
add_subdirectory(ui)
add_subdirectory(utility)