target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ImGuiStyle.cpp
        UIElement.cpp
        UIImage.cpp
        UIPosition.cpp
)

add_subdirectory(editor)
