#pragma once

#include "math/Vector.h"
#include "ui/ImGuiUtility.h"

namespace nc
{
    namespace uistream::detail
    {
        struct Range
        {
            static constexpr auto Default     = Vector2{-500.0f, 500.0f};
            static constexpr auto Nonnegative = Vector2{0.0f, 500.0f};
            static constexpr auto Position    = Vector2{-2000.0f, 2000.0f};
            static constexpr auto Scale       = Vector2{0.0f, 1000.0f};
            static constexpr auto Angle       = Vector2{-2.0f, 2.0f} * std::numbers::pi_v<float>;
        };

        struct StreamState
        {
            PropertyBase currentPropertyBase = PropertyBase{};
            int calls = 0;
            bool hasWrittenValue = false;
        };
    };

    /** Serializes types into ImGui controls. Inputs must be registered types (see type/Type.h). */
    class UIStream
    {
        using StreamState = uistream::detail::StreamState;
        using Range = uistream::detail::Range;

        public:
            /** Render UI control for a type. */
            template<class T>
            static void Draw(T& obj);

            /** Reset stream state. */
            static void Clear() noexcept;

            /** Check if a write has occurred since the last clear. */
            static bool GetWriteStatus() noexcept;

            /** Clear only the write flag from the stream state. */
            static void ClearWriteStatus() noexcept;

            /** Clear the write flag and return the old value. */
            static bool ExchangeWriteStatus() noexcept;

        private:
            inline static StreamState m_state = StreamState{};

            static auto CurrentPropertyName() noexcept;
            static auto GetRange() noexcept -> Vector2;
            static void Draw(float& obj);
            static void Draw(int& obj);
            static void Draw(nc::Vector3& obj);
    };

    template<class T>
    void UIStream::Draw(T& obj)
    {
        ++m_state.calls;
        ImGui::Text(Type<T>::name);

        if constexpr(Type<T>::propertyCount != 0)
        {
            using editor::ImGuiId;
            IMGUI_SCOPE(ImGuiId, m_state.calls);
            ForEachMember<T>([&state = m_state, &obj](auto&& property)
            {
                state.currentPropertyBase = property;
                Draw(obj.*(property.member));
            });
        }
    }

    inline void UIStream::Clear() noexcept
    {
        m_state = StreamState{};
    }

    inline bool UIStream::GetWriteStatus() noexcept
    {
        return m_state.hasWrittenValue;
    }

    inline bool UIStream::ExchangeWriteStatus() noexcept
    {
        return std::exchange(m_state.hasWrittenValue, false);
    }

    inline void UIStream::ClearWriteStatus() noexcept
    {
        m_state.hasWrittenValue = false;
    }

    inline auto UIStream::CurrentPropertyName() noexcept
    {
        return m_state.currentPropertyBase.name;
    }

    inline auto UIStream::GetRange() noexcept -> Vector2
    {
        const auto flags = m_state.currentPropertyBase.flags;
        if(flags & PropertyFlags::Position)    return Range::Position;
        if(flags & PropertyFlags::Scale)       return Range::Scale;
        if(flags & PropertyFlags::Angles)      return Range::Angle;
        if(flags & PropertyFlags::Nonnegative) return Range::Nonnegative;
        return Range::Default;
    }

    inline void UIStream::Draw(float& obj)
    {
        auto [min, max] = GetRange();
        m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragFloat(CurrentPropertyName(), &obj, 1.0f, min, max);
    }

    inline void UIStream::Draw(int& obj)
    {
        m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragInt(CurrentPropertyName(), &obj);
    }

    inline void UIStream::Draw(nc::Vector3& obj)
    {
        auto [min, max] = GetRange();
        m_state.hasWrittenValue = m_state.hasWrittenValue || ImGui::DragFloat3(CurrentPropertyName(), &obj.x, 1.0f, min, max);
    }
}