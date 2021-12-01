#pragma once

#include "imgui/imgui.h"

namespace nc::editor
{
    template<class T>
    struct DragAndDrop{};

    template<>
    struct DragAndDrop<Entity>
    {
        using value_type = Entity;
        static constexpr const char* tag = "NC_DND_ENTITY";
        static constexpr const char* msg = "Entity";
    };

    template<>
    struct DragAndDrop<Camera>
    {
        using value_type = Camera;
        static constexpr const char* tag = "NC_DND_CAMERA";
        static constexpr const char* msg = "Camera";
    };

    template<class T>
    void DragAndDropSource(T* item)
    {
        using dnd = DragAndDrop<T>;
        if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload(dnd::tag, item, sizeof(typename dnd::value_type));
            ImGui::Text(dnd::msg);
            ImGui::EndDragDropSource();
        }
    }

    template<class T, class F>
    void DragAndDropTarget(F&& func)
    {
        using dnd = DragAndDrop<T>;
        if(ImGui::BeginDragDropTarget())
        {
            if(const auto* payload = ImGui::AcceptDragDropPayload(dnd::tag))
            {
                func(static_cast<typename dnd::value_type*>(payload->Data));
            }

            ImGui::EndDragDropTarget();
        }
    }
}