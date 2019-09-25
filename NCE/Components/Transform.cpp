#include "Transform.hpp"

namespace NCE::Components
{       
    Transform::Transform(Common::EntityWeakPtr parent_) : Component(parent_)
    {
        TypeId = ID_TRANSFORM;
        m_rect = NCE::Common::Vector4::Zero();
    }

    int Transform::GetX() const { return m_rect.GetX(); }
    int Transform::GetY() const { return m_rect.GetY(); }
    int Transform::GetW() const { return m_rect.GetZ(); }
    int Transform::GetH() const { return m_rect.GetW(); }

    const NCE::Common::Vector4 Transform::GetVector4() const { return m_rect; }
    const NCE::Common::Rect Transform::GetRect() const
    {
        return NCE::Common::Rect(m_rect.GetX(), m_rect.GetY(), m_rect.GetZ(), m_rect.GetW());
    }

    void Transform::Set(int x_, int y_, int w_, int h_) 
    {
        m_rect = NCE::Common::Vector4(x_, y_, w_, h_);
    }

    void Transform::SetPosition(int x_, int y_) 
    {
        m_rect = NCE::Common::Vector4(x_, y_, m_rect.GetZ(), m_rect.GetW());
    }
        
    void Transform::SetDimensions(int w_, int h_) 
    {
        m_rect = NCE::Common::Vector4(m_rect.GetX(), m_rect.GetY(), w_, h_);
    }

    void Transform::Translate(NCE::Common::Vector2 &v_) 
    {
        auto translation = v_.ToVector4();
        m_rect.TranslateBy(translation);
    }

}