#include "component/Transform.h"
#include "debug/Utils.h"
#include <limits>

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
    using namespace DirectX;
    using namespace nc;

    XMVECTOR ToXMVector(const Vector3& v)            { return XMVectorSet(v.x, v.y, v.z, 0.0f); }
    XMVECTOR ToXMVectorHomogeneous(const Vector3& v) { return XMVectorSet(v.x, v.y, v.z, 1.0f); }
    XMVECTOR ToXMVector(const Quaternion& q)         { return XMVectorSet(q.x, q.y, q.z, q.w); }
    XMMATRIX ToTransMatrix(const Vector3& v)         { return XMMatrixTranslation(v.x, v.y, v.z); }
    XMMATRIX ToScaleMatrix(const Vector3& v)         { return XMMatrixScaling(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Vector3& v)           { return XMMatrixRotationRollPitchYaw(v.x, v.y, v.z); }
    XMMATRIX ToRotMatrix(const Quaternion& q)        { return XMMatrixRotationQuaternion(ToXMVector(q)); }
    XMMATRIX ToRotMatrix(const Vector3& a, float r)  { return XMMatrixRotationAxis(ToXMVector(a), r); }

    XMMATRIX ComposeMatrix(const Vector3& scale, const Quaternion& rot, const Vector3& pos)
    {
        return ToScaleMatrix(scale) * ToRotMatrix(rot) * ToTransMatrix(pos);
    }
}

namespace nc
{
    Transform::Transform(EntityHandle handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale)
        : ComponentBase(handle),
          m_matrix{ComposeMatrix(scale, rot, pos)}
    {
        IF_THROW(!HasNoZeroElement(scale), "Transform::Transform - Invalid scale(elements cannot be 0)");
    }

    Vector3 Transform::GetPosition() const
    {
        Vector3 out;
        DirectX::XMStoreVector3(&out, m_matrix.r[3]);
        return out;
    }

    Quaternion Transform::GetRotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, rot_v);
        return out;
    }

    Vector3 Transform::GetScale() const
    {
        using namespace DirectX;
        // Fill out_v with x scale, then shift in y and z values
        auto out_v = XMVectorSplatX(XMVector3Length(m_matrix.r[0]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(out_v, XMVector3Length(m_matrix.r[1]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0W>(out_v, XMVector3Length(m_matrix.r[2]));
        Vector3 out;
        XMStoreVector3(&out, out_v);
        return out;
    }

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

    Vector3 Transform::ToLocalSpace(const Vector3& vec) const
    {
        auto vec_v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
        auto out_v = DirectX::XMVector3Transform(vec_v, m_matrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_matrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    /** @note It is slighly faster to use dx identity globals instead of the vector
     *  set used in ToLocalSpace, so some code is repeated in Up/Forward/Right. */
    Vector3 Transform::Up() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR1, m_matrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_matrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    Vector3 Transform::Forward() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m_matrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_matrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    Vector3 Transform::Right() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR0, m_matrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_matrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    void Transform::Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale)
    {
        IF_THROW(!HasNoZeroElement(scale), "Transform::Set - Invalid scale(elements cannot be 0)");
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(quat) * ToTransMatrix(pos);
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        IF_THROW(!HasNoZeroElement(scale), "Transform::Set - Invalid scale(elements cannot be 0)");
        m_matrix = ToScaleMatrix(scale) * ToRotMatrix(angles) * ToTransMatrix(pos);
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_matrix.r[3] = ToXMVectorHomogeneous(pos);
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
        IF_THROW(!HasNoZeroElement(scale), "Transform::SetScale - Invalid scale(elements cannot be 0)");
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
            trans_v = DirectX::XMVectorAndInt(trans_v, DirectX::g_XMMask3); //zero w component
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

    #ifdef NC_EDITOR_ENABLED
    void Transform::EditorGuiElement()
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_matrix);
        Vector3 scl, pos;
        auto rot = Quaternion::Identity();
        DirectX::XMStoreVector3(&scl, scl_v);
        DirectX::XMStoreQuaternion(&rot, rot_v);
        DirectX::XMStoreVector3(&pos, pos_v);

        auto angles = rot.ToEulerAngles();

        ImGui::Text("Transform");
        ui::editor::xyzWidgetHeader("   ");
        auto posResult = ui::editor::xyzWidget("Pos", "transformpos", &pos.x, &pos.y, &pos.z);
        auto rotResult = ui::editor::xyzWidget("Rot", "transformrot", &angles.x, &angles.y, &angles.z, std::numbers::pi * -2.0f, std::numbers::pi * 2.0f);
        auto sclResult = ui::editor::xyzWidget("Scl", "transformscl", &scl.x, &scl.y, &scl.z);

        if(posResult)
            SetPosition(pos);
        if(rotResult)
            SetRotation(angles);
        if(sclResult)
            SetScale(scl);
    }
    #endif
}