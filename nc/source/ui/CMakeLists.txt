target_sources(${ENGINE_LIB}
    PRIVATE
        ${ROOT_DIR}/source/ui/UIElement.cpp
        ${ROOT_DIR}/source/ui/UI.cpp
        ${ROOT_DIR}/source/ui/UIImpl.cpp
        ${ROOT_DIR}/source/ui/UIPosition.cpp
)

add_subdirectory(editor)