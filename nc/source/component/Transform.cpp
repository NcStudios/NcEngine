#include "component/Transform.h"
#include "DebugUtils.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
    using namespace DirectX;
    using namespace nc;

    XMVECTOR ToXMVector(const Vector3& v)           { return XMVectorSet(v.x, v.y, v.z, 1.0f); }
    XMVECTOR ToXMVector(const Quaternion& q)        { return XMVectorSet(q.x, q.y, q.z, q.w); }
    XMMATRIX ToTransMatrix(const Vector3& v)        { return XMMatrixTranslation(v.x, v.y, v.z); }
    XMMATRIX ToScaleMatrix(const Vector3& v)        { return XMMatrixScaling(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Vector3& v)          { return XMMatrixRotationRollPitchYaw(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Quaternion& q)       { return XMMatrixRotationQuaternion(ToXMVector(q)); }
    XMMATRIX ToRotMatrix(const Vector3& a, float r) { return XMMatrixRotationAxis(ToXMVector(a), r); }

    XMMATRIX ComposeMatrix(const Vector3& scale, const Quaternion& rot, const Vector3& pos)
    {
        return ToScaleMatrix(scale) * ToRotMatrix(rot) * ToTransMatrix(pos);
    }
}

namespace nc
{
    Transform::Transform(EntityHandle handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale) noexcept
        : ComponentBase(handle),
          m_matrix{ComposeMatrix(scale, rot, pos)}
    {}

    Vector3 Transform::GetPosition() const
    {
        DirectX::XMFLOAT3 row;
        DirectX::XMStoreFloat3(&row, m_matrix.r[3]);
        return Vector3{row};
    }

    Quaternion Transform::GetRotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        DirectX::XMFLOAT4 quat;
        DirectX::XMStoreFloat4(&quat, rot_v);
        return Quaternion{quat};
    }

    Vector3 Transform::GetScale() const
    {
        using namespace DirectX;
        // Fill out_v with x scale, then shift in y and z values
        auto out_v = XMVectorSplatX(XMVector3Length(m_matrix.r[0]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(out_v, XMVector3Length(m_matrix.r[1]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0W>(out_v, XMVector3Length(m_matrix.r[2]));
        XMFLOAT3 out;
        XMStoreFloat3(&out, out_v);
        return Vector3{out};
    }

    #ifdef NC_EDITOR_ENABLED
    void Transform::EditorGuiElement()
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        DirectX::XMFLOAT3 scl, pos;
        DirectX::XMFLOAT4 rot;
        DirectX::XMStoreFloat3(&scl, scl_v);
        DirectX::XMStoreFloat4(&rot, rot_v);
        DirectX::XMStoreFloat3(&pos, pos_v);

        auto angles = Quaternion{rot}.ToEulerAngles();

        ImGui::Text("Transform");
        ui::editor::xyzWidgetHeader("   ");
        auto posResult = ui::editor::xyzWidget("Pos", "transformpos", &pos.x, &pos.y, &pos.z);
        auto rotResult = ui::editor::xyzWidget("Rot", "transformrot", &angles.x, &angles.y, &angles.z, std::numbers::pi * -2.0f, std::numbers::pi * 2.0f);
        auto sclResult = ui::editor::xyzWidget("Scl", "transformscl", &scl.x, &scl.y, &scl.z);

        if(posResult)
            SetPosition(Vector3{pos});
        if(rotResult)
            SetRotation(angles);
        if(sclResult)
            SetScale(Vector3{scl});
    }
    #endif
    
    DirectX::FXMMATRIX Transform::GetTransformationMatrix() const
    {
        return m_matrix;
    }

    DirectX::XMMATRIX Transform::GetTransformationMatrixEx(Vector3 additionalScale) const
    {
        IF_THROW(additionalScale == Vector3::Zero(), "Transform::GetTransformationMatrixEx - Scale cannot be zero");
        return ToScaleMatrix(additionalScale) * m_matrix;
    }

    DirectX::XMMATRIX Transform::GetViewMatrix() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, DirectX::g_XMIdentityR1);
    }

    void Transform::Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Transform::Set - Invalid scale(Vector3::Zero)");
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(quat) * ToTransMatrix(pos);
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Transform::Set - Invalid scale(Vector3::Zero)");
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(angles) * ToTransMatrix(pos);
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_matrix.r[3] = ToXMVector(pos);
    }

    void Transform::SetRotation(const Quaternion& quat)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   ToRotMatrix(quat) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetRotation(const Vector3& angles)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                   ToRotMatrix(angles) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::SetScale(const Vector3& scale)
    {
        IF_THROW(scale == Vector3::Zero(), "Transform::SetScale - Invalid scale(Vector3::Zero)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        m_matrix = ToScaleMatrix(scale) *
                   DirectX::XMMatrixRotationQuaternion(rot_v) *
                   DirectX::XMMatrixTranslationFromVector(pos_v);
    }

    void Transform::Translate(Vector3 translation, Space space)
    {
        auto trans_v = ToXMVector(translation);
        if(space == Space::Local)
        {
            DirectX::XMVECTOR pos_v, rot_v, scl_v;
            DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
            trans_v = DirectX::XMVector3Transform(trans_v, DirectX::XMMatrixRotationQuaternion(rot_v));
        }
        m_matrix.r[3] += trans_v;
    }

    void Transform::Rotate(const Quaternion& quat)
    {
        auto pos_v = m_matrix.r[3];
        m_matrix.r[3] = DirectX::g_XMIdentityR3;
        m_matrix *= ToRotMatrix(quat);
        m_matrix.r[3] = pos_v;
    }

    void Transform::Rotate(Vector3 axis, float radians)
    {
        auto pos_v = m_matrix.r[3];
        m_matrix.r[3] = DirectX::g_XMIdentityR3;
        m_matrix *= ToRotMatrix(axis, radians);
        m_matrix.r[3] = pos_v;
    }
}