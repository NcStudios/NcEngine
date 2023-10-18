target_sources(${NC_ENGINE_LIB}
    PRIVATE
        ImGuiUtility.cpp
        UIElement.cpp
        UIPosition.cpp
)

add_subdirectory(editor)
