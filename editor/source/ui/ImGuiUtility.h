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
    constexpr float DefaultItemWidth = 50.0f;

    /** RAII Properties */
    struct ImGuiId;
    struct Indent;
    struct ItemWidth;
    struct StyleColor;

    inline void SameLineSpaced();
    inline void SeparatorSpaced();
    inline bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);
    inline bool InputVector3(const char* label, Vector3* vec);
    inline bool InputVector3(const char* label, Vector3* vec, float speed = 1.0f, float min = 0.0f, float max = 100.0f);
    inline bool InputPosition(const char* label, Vector3* position);
    inline bool InputAngles(const char* label, Vector3* angles);
    inline bool InputScale(const char* label, Vector3* scale);

    template<class T>
    void DragAndDropSource(T* item);

    template<class T, std::invocable<T*> F>
    void DragAndDropTarget(F&& func);

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
        explicit ImGuiId(int id)         { ImGui::PushID(id); }
        ~ImGuiId() noexcept              { ImGui::PopID();    }
    };

    struct Indent
    {
        explicit Indent()  { ImGui::Indent(); }
        ~Indent() noexcept { ImGui::Unindent(); }
    };

    struct StyleColor
    {
        explicit StyleColor(ImGuiCol index, ImU32 col)          { ImGui::PushStyleColor(index, col); }
        explicit StyleColor(ImGuiCol index, const ImVec4& col)  { ImGui::PushStyleColor(index, col); }
        explicit StyleColor(ImGuiCol index, const Vector4& col) { ImGui::PushStyleColor(index, col); }
        ~StyleColor() noexcept                                  { ImGui::PopStyleColor();            }
    };

    struct ItemWidth
    {
        explicit ItemWidth(float itemWidth) { ImGui::PushItemWidth(itemWidth); }
        ~ItemWidth() noexcept               { ImGui::PopItemWidth();           }
    };

    void SameLineSpaced()
    {
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
    }

    void SeparatorSpaced()
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
    {
        flags |= ImGuiInputTextFlags_CallbackResize;
        internal::InputTextCallbackUserData userData{.string = str};
        return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, internal::InputTextCallback, &userData);
    }

    bool InputVector3(const char* label, Vector3* vec)
    {
        return ImGui::DragFloat3(label, &(vec->x));
    }

    bool InputVector3(const char* label, Vector3* vec, float speed, float min, float max)
    {
        return ImGui::DragFloat3(label, &(vec->x), speed, min, max);
    }

    bool InputPosition(const char* label, Vector3* position)
    {
        return ImGui::DragFloat3(label, &(position->x), 1.0f, -1000.0f, 1000.0f);
    }

    bool InputAngles(const char* label, Vector3* angles)
    {
        return ImGui::DragFloat3(label, &(angles->x), 0.1f, std::numbers::pi * -2.0f, std::numbers::pi * 2.0f);
    }

    bool InputScale(const char* label, Vector3* scale)
    {
        return ImGui::DragFloat3(label, &(scale->x), 0.5f, 0.01f, 1000.0f);
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