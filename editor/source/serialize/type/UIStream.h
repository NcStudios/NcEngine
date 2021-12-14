#pragma once

#include "Type.h"

#include "ui/ImGuiUtility.h"

#include "math/Vector.h"

namespace nc
{
    struct Color3
    {
        float r, g, b;

        Color3() : r{0.0f}, g{0.0f}, b{0.0f} {}
        Color3(const Vector3& v) : r{v.x}, g{v.y}, b{v.z} {}
        operator Vector3() { return Vector3{r, g, b}; }
    };

    constexpr auto DefaultRange     = Vector2{-500.0f, 500.0f};
    constexpr auto NonnegativeRange = Vector2{0.0f, 500.0f};
    constexpr auto PositionRange    = Vector2{-2000.0f, 2000.0f};
    constexpr auto ScaleRange       = Vector2{0.0f, 1000.0f};
    constexpr auto AngleRange       = Vector2{-2.0f, 2.0f} * std::numbers::pi;


// struct UIStreamIndentation
// {
//     UIStreamIndentation() : amount{0u} {}
//     UIStreamIndentation(size_t count) : amount{count} {}

//     UIStreamIndentation& operator +=(UIStreamIndentation other)
//     {
//         amount += other.amount;
//         return *this;
//     }

//     UIStreamIndentation& operator -=(UIStreamIndentation other)
//     {
//         amount > other.amount ?
//             amount -= other.amount :
//             amount = 0u;

//         return *this;
//     }

//     size_t amount;
// };

struct UIStreamState
{
    PropertyBase currentPropertyBase = PropertyBase{};
    int calls = 0;
    bool hasWrittenValue = false;
};

class UIStream
{
    public:
        template<class T>
        static void Send(T& obj)
        {
            ++m_state.calls;
            constexpr auto propertyCount = std::tuple_size<decltype(Meta<T>::properties)>::value;
            ImGui::Text(Meta<T>::name);

            if constexpr(propertyCount != 0)
            {
                using editor::ImGuiId;
                IMGUI_SCOPE(ImGuiId, m_state.calls);

                auto f = [&state = m_state, &obj](auto&& property)
                {
                    state.currentPropertyBase = property;
                    Send(obj.*(property.member));
                };

                ForEachMember<T>(f);
            }
        }

        static void Clear()               { m_state = UIStreamState{}; }
        static bool GetWriteStatus()      { return m_state.hasWrittenValue; }
        static bool ExchangeWriteStatus() { return std::exchange(m_state.hasWrittenValue, false); }
        static void ClearWriteStatus()    { m_state.hasWrittenValue = false; }

    private:
        inline static UIStreamState m_state = UIStreamState{};

        static auto CurrentPropertyName()  { return m_state.currentPropertyBase.name; }

        static auto GetRange() -> Vector2
        {
            const auto flags = m_state.currentPropertyBase.flags;
            if(flags & SerializationFlags::Position)    return PositionRange;
            if(flags & SerializationFlags::Scale)       return ScaleRange;
            if(flags & SerializationFlags::Angles)      return AngleRange;
            if(flags & SerializationFlags::Nonnegative) return NonnegativeRange;
            return DefaultRange;
        }

        static void Send(float& obj)
        {
            auto [min, max] = GetRange();
            m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragFloat(CurrentPropertyName(), &obj, 1.0f, min, max);
        }

        static void Send(int& obj)
        {
            m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragInt(CurrentPropertyName(), &obj);
        }

        static void Send(nc::Vector3& obj)
        {
            m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragFloat3(CurrentPropertyName(), &obj.x);
        }

        static void Send(nc::Color3& obj)
        {
            m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::ColorEdit3(CurrentPropertyName(), &obj.r, ImGuiColorEditFlags_NoInputs);
        }
};

// class UIStream2
// {
//     public:
//         template<class T>
//         friend UIStream2& operator << (UIStream2& s, T&)
//         {
//             return s;
//         }
// };

// template<>
// UIStream2& operator <<(UIStream2& s, float&)
// {
//     return s;
// }

}