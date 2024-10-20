/**
 * @file ImGuiUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 * 
 * @note Unless otherwise noted, all functions in this file must only be called
 *       during UI rendering. Calls should be made only from within IUI::Draw()
 *       implementations or from ComponentHandler<T>::drawUI callbacks.
 */

#pragma once

#include "ncengine/type/EngineTypes.h"
#include "ncengine/ui/ImGuiConversion.h"

#include "imgui/imgui.h"
#include "ncmath/Vector.h"
#include "ncmath/Geometry.h"

#include <algorithm>
#include <functional>
#include <numbers>
#include <span>
#include <string>
#include <string_view>

/** @cond internal */
#define IMGUI_SCOPE_UNIQUE_NAME_HELPER(name, lineNumber) name ## lineNumber
#define IMGUI_SCOPE_UNIQUE_NAME(name, lineNumber) IMGUI_SCOPE_UNIQUE_NAME_HELPER(name, lineNumber)
/** @endcond */

/** @brief Create a variable to hold a scoped property. */
#define IMGUI_SCOPE(Property, ...) auto IMGUI_SCOPE_UNIQUE_NAME(imguiProp, __LINE__) = Property(__VA_ARGS__)

namespace nc::ui
{
constexpr auto g_defaultItemWidth = 50.0f;
constexpr auto g_maxPos = 5000.0f;
constexpr auto g_minPos = -g_maxPos;
constexpr auto g_maxAngle = std::numbers::pi_v<float> * 2.0f;
constexpr auto g_minAngle = -g_maxAngle;
constexpr auto g_minScale = 0.1f;
constexpr auto g_maxScale = 1000.0f;

/** @brief Create a top-level window. */
template<class F>
void Window(const char* label, F&& drawContents);

/** @brief Create a top-level window. */
template<class F>
void Window(const char* label, ImGuiWindowFlags flags, F&& drawContents);

/** @brief Create a child window. */
template<class F>
void ChildWindow(const char* label, F&& drawContents);

/** @brief Check if the current window background is clicked on. */
auto IsWindowBackgroundClicked() -> bool;

/** @brief Input field UI widget for int. */
auto InputInt(int& value, const char* label, int step = 1) -> bool;

/** @brief Input field UI widget for uint8_t. */
auto InputU8(uint8_t& value, const char* label) -> bool;

/** @brief Input field UI widget for uint16_t. */
auto InputU16(uint16_t& value, const char* label) -> bool;

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
auto InputScale(Vector3& value, const char* label, float min = g_minScale, float max = g_maxScale) -> bool;

/** @brief Vector3 UI widget constraint for normalized axis inputs. */
auto InputAxis(Vector3& value, const char* label, float min = -1.0f, float max = 1.0f) -> bool;

/** @brief RGB color picker UI widget. */
auto InputColor3(Vector3& value, const char* label) -> bool;

/** @brief RGBA color picker UI widget. */
auto InputColor4(Vector4& value, const char* label) -> bool;

/** @brief Combobox UI widget. */
auto Combobox(std::string& value, const char* label, std::span<const std::string_view> items);

/** @brief Text input UI widget. */
auto InputText(std::string& value, const char* label) -> bool;

/** @brief Wrap a widget with a selectable - allows for things like list box of input widgets. */
auto SelectableWidget(bool selected, const ImVec2& size, auto&& widget) -> bool;

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

/** @brief UI layout helper for right aligning an item on the same line as the previous item. */
void SameLineRightAligned(float itemWidth);

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

/** @brief RAII wrapper for conditionally disabling widgets within a scope. */
struct DisableIf
{
    explicit DisableIf(bool condition) { ImGui::BeginDisabled(condition); }
    ~DisableIf() noexcept              { ImGui::EndDisabled();            }
};

/** @brief Check if the UI is currently using keyboard events.
 *  @note May be called at any time. */
inline auto IsCapturingKeyboard() -> bool
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

/** @brief Check if the UI is currently using mouse events.
 *  @note May be called at any time. */
inline auto IsCapturingMouse() -> bool
{
    return ImGui::GetIO().WantCaptureMouse;
}

template<class F>
void Window(const char* label, ImGuiWindowFlags flags, F&& drawContents)
{
    if (ImGui::Begin(label, nullptr, flags))
    {
        drawContents();
    }

    ImGui::End();
}

template<class F>
void Window(const char* label, F&& drawContents)
{
    Window(label, ImGuiWindowFlags_None, std::forward<F>(drawContents));
}

template<class F>
void ChildWindow(const char* label, F&& drawContents)
{
    if (ImGui::BeginChild(label, {0, 0}, true))
    {
        drawContents();
    };

    ImGui::EndChild();
}

inline auto IsWindowBackgroundClicked() -> bool
{
    return ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered();
}

inline void SameLineSpaced()
{
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
}

inline void SameLineRightAligned(float itemWidth)
{
    const auto availableWidth = ImGui::GetContentRegionAvail().x;
    itemWidth += + ImGui::GetStyle().FramePadding.x * 2.0f;
    ImGui::SameLine(ImGui::GetCursorPosX() + availableWidth - itemWidth);
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

inline auto InputU8(uint8_t& value, const char* label) -> bool
{
    constexpr uint8_t step = 1;
    return ImGui::InputScalar(label, ImGuiDataType_U8, &value, &step);
}

inline auto InputU16(uint16_t& value, const char* label) -> bool
{
    constexpr uint16_t step = 1;
    return ImGui::InputScalar(label, ImGuiDataType_U16, &value, &step);
}

inline auto InputU32(unsigned& value, const char* label) -> bool
{
    constexpr uint32_t step = 1;
    return ImGui::InputScalar(label, ImGuiDataType_U32, &value, &step);
}

inline auto InputU64(uint64_t& value, const char* label) -> bool
{
    constexpr uint64_t step = 1;
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

inline auto InputScale(Vector3& value, const char* label, float min, float max) -> bool
{
    return ImGui::DragFloat3(label, &value.x, 0.5f, min, max);
}

inline auto InputAxis(Vector3& value, const char* label, float min, float max) -> bool
{
    const auto previous = value;
    if (ImGui::DragFloat3(label, &value.x, 0.1f, min, max))
    {
        // When a component is changed to a maximum, other values are still potentially non-zero. These cases need to
        // be hard reset to the correct axis otherwise normalization prevents ever being able to reach it.
        if (value.x != previous.x && std::fabs(value.x) == 1.0f)
            value = Vector3{value.x, 0.0f, 0.0f};
        else if (value.y != previous.y && std::fabs(value.y) == 1.0f)
            value = Vector3{0.0f, value.y, 0.0f};
        else if (value.z != previous.z && std::fabs(value.z) == 1.0f)
            value = Vector3{0.0f, 0.0f, value.z};
        else if (value == Vector3::Zero()) // prevent problems when 0 is directly enterered
            value = previous;
        else
            value = Normalize(value);

        return true;
    }

    return false;
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
        const auto selected = std::ranges::find_if(items, [](const auto& text)
        {
            return ImGui::Selectable(text.data());
        });

        ImGui::EndCombo();

        if (selected != std::cend(items))
        {
            value = selected->data();
            return true;
        }
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

auto SelectableWidget(bool selected, const ImVec2& size, auto&& widget) -> bool
{
    static constexpr auto flags = ImGuiSelectableFlags_AllowDoubleClick |
                                  ImGuiSelectableFlags_AllowItemOverlap;
    auto clicked = false;
    if (ImGui::Selectable("##select", selected, flags, size))
        clicked = true;

    ImGui::SameLine();
    widget();
    if (ImGui::IsItemClicked(0))
    {
        clicked = true;
        ImGui::SetKeyboardFocusHere(-1);
    }

    return clicked;
};

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
