target_sources(${EXE_NAME}
    PRIVATE
        ${ROOT_DIR}/nc/source/ui/UIElement.cpp
        ${ROOT_DIR}/nc/source/ui/UI.cpp
        ${ROOT_DIR}/nc/source/ui/UIImpl.cpp
        ${ROOT_DIR}/nc/source/ui/UIPosition.cpp
)

add_subdirectory(editor)