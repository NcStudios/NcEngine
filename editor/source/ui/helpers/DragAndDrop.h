#pragma once

#include "imgui/imgui.h"
#include "utility/Type.h"

namespace nc::editor
{
    template<class T>
    void DragAndDropSource(T* item)
    {
        if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload(TypeInfo<T>::name, item, sizeof(T));
            ImGui::Text(TypeInfo<T>::name);
            ImGui::EndDragDropSource();
        }
    }

    template<class T, class F>
    void DragAndDropTarget(F&& func)
    {
        if(ImGui::BeginDragDropTarget())
        {
            if(const auto* payload = ImGui::AcceptDragDropPayload(TypeInfo<T>::name))
            {
                func(static_cast<std::add_pointer_t<T>>(payload->Data));
            }

            ImGui::EndDragDropTarget();
        }
    }
}