#include "Transform.hpp"

namespace NCE::Components
{       
    Transform::Transform(Common::EntityWeakPtr t_parent) : Component(t_parent)
    {
        TypeId = ID_TRANSFORM;
        _rect.Set(0, 0, 0, 0);
    }

    int Transform::GetX() const { return _rect.GetA(); }
    int Transform::GetY() const { return _rect.GetB(); }
    int Transform::GetW() const { return _rect.GetC(); }
    int Transform::GetH() const { return _rect.GetD(); }

    const NCE::Common::Vector4 Transform::GetVector4() const { return _rect; }
    const NCE::Common::Rect Transform::GetRect() const
    {
        return NCE::Common::Rect(_rect.GetA(), _rect.GetB(), _rect.GetC(), _rect.GetD());
    }

    void Transform::Set(int t_x, int t_y, int t_w, int t_h) { _rect.Set(t_x, t_y, t_w, t_h); }

    void Transform::SetPosition(int t_x, int t_y) { _rect.Set(t_x, t_y, _rect.GetC(), _rect.GetD()); }
        
    void Transform::SetDimensions(int t_w, int t_h) { _rect.Set(_rect.GetA(), _rect.GetB(), t_w, t_h); }

    void Transform::Translate(int t_x, int t_y) { _rect.Set(_rect.GetA() + t_x, _rect.GetB() + t_y, _rect.GetC(), _rect.GetD()); }

}