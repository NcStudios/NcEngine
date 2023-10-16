#pragma once

#include "ncengine/type/EngineTypes.h"

#include "imgui/imgui.h"
#include "ncmath/Vector.h"
#include "ncmath/Geometry.h"

#include <functional>
#include <numbers>
#include <span>
#include <string>
#include <string_view>

#define IMGUI_SCOPE_UNIQUE_NAME_HELPER(name, lineNumber) name ## lineNumber
#define IMGUI_SCOPE_UNIQUE_NAME(name, lineNumber) IMGUI_SCOPE_UNIQUE_NAME_HELPER(name, lineNumber)

/** Create a variable to hold a scoped property. */
#define IMGUI_SCOPE(Property, ...) auto IMGUI_SCOPE_UNIQUE_NAME(imguiProp, __LINE__) = Property(__VA_ARGS__)

namespace nc::ui
{
constexpr auto g_defaultItemWidth = 50.0f;
constexpr auto g_maxPos = 5000.0f;
constexpr auto g_minPos = -g_maxPos;
constexpr auto g_maxAngle = std::numbers::pi_v<float> * 2.0f;
constexpr auto g_minAngle = -g_maxAngle;
constexpr auto g_minScale = 0.0001f;
constexpr auto g_maxScale = 1000.0f;

/** @brief Input field UI widget for int. */
auto InputInt(int& value, const char* label, int step = 1) -> bool;

/** @brief Input field UI widget for uint32_t. */
auto InputU32(uint32_t& value, const char* label) -> bool;

/** @brief Input field UI widget for uint64_t. */
auto InputU64(uint64_t& value, const char* label) -> bool;

/** @brief Drag slider UI widget for float. */
auto DragFloat(float& value, const char* label, float speed = 1.0f, float min = 0.0f, float max = 1000.0f) -> bool;

/** @brief Checkbox UI widget for bool. */
auto Checkbox(bool& value, const char* name) -> bool;

/** @brief Vector3 UI widget with three float drag sliders. */
auto InputVector3(Vector3& value, const char* label, float speed = 1.0f, float min = 0.0f, float max = 100.0f) -> bool;

/** @brief Vector3 UI widget constrained for position inputs. */
auto InputPosition(Vector3& value, const char* label) -> bool;

/** @brief Vector3 UI widget constrained for angular (radians) inputs. */
auto InputAngles(Vector3& value, const char* label) -> bool;

/** @brief Vector3 UI widget constrained for scale inputs. */
auto InputScale(Vector3& value, const char* label) -> bool;

/** @brief RGB color picker UI widget. */
auto InputColor3(Vector3& value, const char* label) -> bool;

/** @brief RGBA color picker UI widget. */
auto InputColor4(Vector4& value, const char* label) -> bool;

/** @brief Combobox UI widget. */
auto Combobox(std::string& value, const char* label, std::span<const std::string_view> items);

/** @brief Text input UI widget. */
auto InputText(std::string& value, const char* label) -> bool;

/**
 * @brief Simple getter/setter-based property wrapper.
 * @tparam Get Callable compatible with `value_t (*)(object_t&)`.
 * @tparam Set Callable compatible with `void (*)(object_t&, value_t&).
 */
template<class Getter, class Setter>
struct Property
{
    Getter get;
    Setter set;
    const char* name = "";
};

/**
 * @brief Invoke a widget function with a Property and object instance.
 * @param property A `Property<G, S>` instance to be drawn.
 * @param instance An object instance to query the property from.
 * @param widget The widget function to call.
 * @param args Arguments to `widget`, excluding initial value and name args.
 * @return A bool indicating whether values were modified through the widget.
 * 
 * Invoke a widget function with a Property instead of a direct value. `property`
 * supplies both a value and name for `widget`, so `args` should only contain
 * any additional arguments required by `widget`. On write, `property.set` is
 * called with the updated value.
 */
auto PropertyWidget(const auto& property, auto& instance, auto&& widget, auto&&... args) -> bool;

/** @brief Introduce a drag and drop source with item as its payload. */
template<class T>
void DragAndDropSource(T* item);

/** @brief Introduce a drag and drop target that can accept a payload of type T*. */
template<class T, std::invocable<T*> F>
void DragAndDropTarget(F&& func);

/** @brief UI layout helper for spacing. */
void SameLineSpaced();

/** @brief UI layout helper for separator. */
void SeparatorSpaced();

/** @brief RAII wrapper for scoped uids. */
struct ImGuiId
{
    explicit ImGuiId(const char* id) { ImGui::PushID(id); }
    explicit ImGuiId(int id)         { ImGui::PushID(id); }
    ~ImGuiId() noexcept              { ImGui::PopID();    }
};

/** @brief RAII wrapper for scoped indentation. */
struct Indent
{
    explicit Indent()  { ImGui::Indent(); }
    ~Indent() noexcept { ImGui::Unindent(); }
};

/** @brief RAII wrapper for scoped style color. */
struct StyleColor
{
    explicit StyleColor(ImGuiCol index, ImU32 col)          { ImGui::PushStyleColor(index, col); }
    explicit StyleColor(ImGuiCol index, const ImVec4& col)  { ImGui::PushStyleColor(index, col); }
    explicit StyleColor(ImGuiCol index, const Vector4& col) { ImGui::PushStyleColor(index, col); }
    ~StyleColor() noexcept                                  { ImGui::PopStyleColor();            }
};

/** @brief RAII wrapper for scoped item width. */
struct ItemWidth
{
    explicit ItemWidth(float itemWidth) { ImGui::PushItemWidth(itemWidth); }
    ~ItemWidth() noexcept               { ImGui::PopItemWidth();           }
};

namespace default_scheme
{
constexpr auto Black             = Vector4{0.009f, 0.009f, 0.009f, 1.000f};
constexpr auto White             = Vector4{1.000f, 1.000f, 1.000f, 1.000f};
constexpr auto Clear             = Vector4{0.000f, 0.000f, 0.000f, 0.000f};
constexpr auto TransWhite        = Vector4{1.000f, 1.000f, 1.000f, 0.700f};
constexpr auto Red               = Vector4{1.000f, 0.200f, 0.100f, 1.000f};
constexpr auto Green             = Vector4{0.000f, 1.000f, 0.000f, 1.000f};
constexpr auto Blue              = Vector4{0.000f, 0.400f, 1.000f, 1.000f};
constexpr auto GrayLightest      = Vector4{0.348f, 0.348f, 0.348f, 1.000f};
constexpr auto GrayLight         = Vector4{0.168f, 0.168f, 0.168f, 1.000f};
constexpr auto Gray              = Vector4{0.072f, 0.072f, 0.072f, 1.000f};
constexpr auto GrayDark          = Vector4{0.035f, 0.035f, 0.035f, 1.000f};
constexpr auto GrayDarkest       = Vector4{0.018f, 0.018f, 0.018f, 1.000f};
constexpr auto TransGrayLight    = Vector4{0.740f, 0.740f, 0.740f, 0.200f};
constexpr auto TransGrayLightAlt = Vector4{0.740f, 0.740f, 0.740f, 0.350f};
constexpr auto TransGray         = Vector4{0.410f, 0.410f, 0.410f, 0.500f};
constexpr auto TransGrayAlt      = Vector4{0.610f, 0.610f, 0.610f, 0.390f};
constexpr auto TransGrayDark     = Vector4{0.017f, 0.017f, 0.017f, 0.530f};
constexpr auto AccentDark        = Vector4{0.000f, 0.447f, 0.000f, 1.000f};
constexpr auto Accent            = Vector4{0.000f, 0.490f, 0.000f, 1.000f};
constexpr auto AccentLight       = Vector4{0.000f, 0.729f, 0.000f, 1.000f};
constexpr auto AccentTrans       = Vector4{0.000f, 0.990f, 0.000f, 0.050f};
} // namespace default_scheme

/** @brief Set ImGUi style using values from 'default_scheme'.*/
void SetDefaultUIStyle();

inline void SameLineSpaced()
{
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
}

inline void SeparatorSpaced()
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

inline auto InputInt(int& value, const char* label, int step) -> bool
{
    return ImGui::InputInt(label, &value, step);
}

inline auto InputU32(unsigned& value, const char* label) -> bool
{
    constexpr auto step = 1u;
    return ImGui::InputScalar(label, ImGuiDataType_U32, &value, &step);
}

inline auto InputU62(unsigned& value, const char* label) -> bool
{
    constexpr auto step = 1ull;
    return ImGui::InputScalar(label, ImGuiDataType_U64, &value, &step);
}

inline auto DragFloat(float& value, const char* label, float speed, float min, float max) -> bool
{
    return ImGui::DragFloat(label, &value, speed, min, max);
}

inline auto Checkbox(bool& value, const char* name) -> bool
{
    return ImGui::Checkbox(name, &value);
}

inline auto InputVector3(Vector3& value, const char* label, float speed, float min, float max) -> bool
{
    return ImGui::DragFloat3(label, &value.x, speed, min, max);
}

inline auto InputPosition(Vector3& value, const char* label) -> bool
{
    return ImGui::DragFloat3(label, &value.x, 1.0f, g_minPos, g_maxPos);
}

inline auto InputAngles(Vector3& value, const char* label) -> bool
{
    return ImGui::DragFloat3(label, &value.x, 0.1f, g_minAngle, g_maxAngle);
}

inline auto InputScale(Vector3& value, const char* label) -> bool
{
    return ImGui::DragFloat3(label, &value.x, 0.5f, g_minScale, g_maxScale);
}

inline auto InputColor(Vector3& value, const char* label) -> bool
{
    return ImGui::ColorEdit3(label, &value.x, ImGuiColorEditFlags_NoInputs);
}

inline auto InputColor3(Vector3& value, const char* label) -> bool
{
    return ImGui::ColorEdit3(label, &value.x, ImGuiColorEditFlags_NoInputs);
}

inline auto InputColor4(Vector4& value, const char* label) -> bool
{
    return ImGui::ColorEdit4(label, &value.x, ImGuiColorEditFlags_NoInputs);
}

inline auto Combobox(std::string& value, const char* label, std::span<const std::string_view> items)
{
    if (ImGui::BeginCombo(label, value.c_str()))
    {
        for (const auto& path : items)
        {
            if (ImGui::Selectable(path.data()))
            {
                value = path.data();
                return true;
            }
        }

        ImGui::EndCombo();
    }

    return false;
}

namespace internal
{
struct InputTextCallbackUserData
{
    std::string* string;
};

inline auto InputTextCallback(ImGuiInputTextCallbackData* data) -> int
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
} // namespace internal

inline auto InputText(std::string& text, const char* label) -> bool
{
    auto flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue;
    auto userData = internal::InputTextCallbackUserData{.string = &text};
    return ImGui::InputText(label, text.data(), text.capacity() + 1, flags, internal::InputTextCallback, &userData);
}

auto PropertyWidget(const auto& property, auto& instance, auto&& widget, auto&&... args) -> bool
{
    auto value = std::invoke(property.get, instance);
    if (widget(value, property.name, std::forward<decltype(args)>(args)...))
    {
        std::invoke(property.set, instance, value);
        return true;
    }

    return false;
}

template<class T>
void DragAndDropSource(T* item)
{
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload(type::Type<T>::name, item, sizeof(T));
        ImGui::Text(type::Type<T>::name);
        ImGui::EndDragDropSource();
    }
}

template<class T, std::invocable<T*> F>
void DragAndDropTarget(F&& func)
{
    if(ImGui::BeginDragDropTarget())
    {
        if(const auto* payload = ImGui::AcceptDragDropPayload(type::Type<T>::name))
        {
            func(static_cast<std::add_pointer_t<T>>(payload->Data));
        }

        ImGui::EndDragDropTarget();
    }
}
} // namespace nc::ui
