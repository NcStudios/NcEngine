#ifndef TRANSFORM
#define TRANSFORM


#include "Common.h"
#include "Component.h"
#include "Vector.h"

class Transform : public Component
{
    private:
        Vector4 m_rect;
        bool m_physicsEnabled;
        bool m_renderingEnabled;
        //pointer to Sprite

    public:
        Transform(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle), m_physicsEnabled(false) {}
        Vector4 GetRect() const { return m_rect; }
        Vector2 GetPosition() const { return Vector2(m_rect.GetX(), m_rect.GetY()); }
        double GetWidth() const { return m_rect.GetZ(); }
        double GetHeight() const { return m_rect.GetW(); }

        void SetRect(const Vector4& rect) { m_rect = rect; }
        void SetPosition(const Vector2& pos) { m_rect = Vector4(pos.GetX(), pos.GetY(), m_rect.GetZ(), m_rect.GetW()); }
        void Translate(const Vector2& vec) { m_rect.TranslateBy(vec); }

        bool IsPhysicsEnabled() const { return m_physicsEnabled; }
        bool IsRenderingEnabled() const { return m_renderingEnabled; }

        void TogglePhysicsEnabled(bool state) { m_physicsEnabled = state; }
        void ToggleRenderingEnabled(bool state) { m_renderingEnabled = state; }
};

#endif