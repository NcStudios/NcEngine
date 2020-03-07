#pragma once
#include "Common.h"
#include "Component.h"
#include "NCVector.h"

namespace nc
{
    class Transform : public Component
    {
        private:
            Vector4 m_rect;
            bool m_physicsEnabled;
            bool m_renderingEnabled;

        public:
            Transform(ComponentHandle handle, EntityHandle parentHandle) noexcept : Component(handle, parentHandle), m_physicsEnabled(false), m_renderingEnabled(false) {}
            Vector4 GetRect() const noexcept { return m_rect; }
            Vector2 GetPosition() const noexcept { return Vector2(m_rect.GetX(), m_rect.GetY()); }
            double GetWidth() const noexcept { return m_rect.GetZ(); }
            double GetHeight() const noexcept { return m_rect.GetW(); }

            void SetRect(const Vector4& rect) noexcept { m_rect = rect; }
            void SetPosition(const Vector2& pos) noexcept { m_rect = Vector4(pos.GetX(), pos.GetY(), m_rect.GetZ(), m_rect.GetW()); }
            void Translate(const Vector2& vec) noexcept { m_rect.TranslateBy(vec); }

            bool IsPhysicsEnabled() const noexcept { return m_physicsEnabled; }
            bool IsRenderingEnabled() const noexcept { return m_renderingEnabled; }
            void TogglePhysicsEnabled(bool state) noexcept { m_physicsEnabled = state; }
            void ToggleRenderingEnabled(bool state) noexcept { m_renderingEnabled = state; }
    };
} //end namespace nc