target_sources(NcEngineLib
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ui/UIElement.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/UISystemImpl.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/UIPosition.cpp
)

add_subdirectory(editor)