#pragma once

#include "utility/Type.h"
#include "imgui/imgui.h"

/** Create a variable to hold a scoped property. */
#define IMGUI_SCOPE(Property, ...) auto property_##Property = Property(__VA_ARGS__)

/** When multiple properties of the same type are needed in the same scope, use this
 *  and give a unique id to each. */
#define IMGUI_SCOPE_ID(Property, Id, ...) auto property_##Property##Id = Property(__VA_ARGS__)

namespace nc::editor
{
    /** RAII Properties */
    struct ImGuiId;
    struct ItemWidth;
    struct StyleColor;

    inline bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);

    template<class T>
    void DragAndDropSource(T* item);

    template<class T, std::invocable<T*> F>
    void DragAndDropTarget(F&& func);

    /** @todo Various vector inputs */

    namespace internal
    {
        struct InputTextCallbackUserData
        {
            std::string* string;
        };

        inline int InputTextCallback(ImGuiInputTextCallbackData* data)
        {
            if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                InputTextCallbackUserData* userData = static_cast<InputTextCallbackUserData*>(data->UserData);
                auto* str = userData->string;
                str->resize(data->BufTextLen);
                data->Buf = (char*)str->c_str();
            }

            return 0;
        }
    }

    struct ImGuiId
    {
        explicit ImGuiId(const char* id) { ImGui::PushID(id); }
        ~ImGuiId() noexcept              { ImGui::PopID();    }
    };

    struct StyleColor
    {
        explicit StyleColor(ImGuiCol index, ImU32 col)         { ImGui::PushStyleColor(index, col); }
        explicit StyleColor(ImGuiCol index, const ImVec4& col) { ImGui::PushStyleColor(index, col); }
        ~StyleColor() noexcept                                 { ImGui::PopStyleColor();            }
    };

    struct ItemWidth
    {
        explicit ItemWidth(float itemWidth) { ImGui::PushItemWidth(itemWidth); }
        ~ItemWidth() noexcept               { ImGui::PopItemWidth();           }
    };

    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
    {
        flags |= ImGuiInputTextFlags_CallbackResize;
        internal::InputTextCallbackUserData userData{.string = str};
        return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, internal::InputTextCallback, &userData);
    }

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

    template<class T, std::invocable<T*> F>
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