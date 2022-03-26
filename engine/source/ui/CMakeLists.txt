target_sources(NcEngineLib
    PRIVATE
        ${ROOT_DIR}/source/ui/UIElement.cpp
        ${ROOT_DIR}/source/ui/UISystemImpl.cpp
        ${ROOT_DIR}/source/ui/UIPosition.cpp
)

add_subdirectory(editor)