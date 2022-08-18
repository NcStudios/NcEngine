#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "debug/Utils.h"

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
    Transform::Transform(Entity entity, const Vector3& pos, const Quaternion& rot, const Vector3& scale, Entity parent)
        : ComponentBase(entity),
          m_localMatrix{ComposeMatrix(scale, rot, pos)},
          m_worldMatrix{m_localMatrix},
          m_parent{parent},
          m_children{}
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        if(m_parent.Valid())
        {
            auto* parentTransform = ActiveRegistry()->Get<Transform>(parent);
            parentTransform->AddChild(entity);
            m_worldMatrix = parentTransform->TransformationMatrix() * m_worldMatrix;
        }
    }

    Vector3 Transform::LocalPosition() const
    {
        Vector3 out;
        DirectX::XMStoreVector3(&out, m_localMatrix.r[3]);
        return out;
    }

    Vector3 Transform::Position() const
    {
        Vector3 out;
        DirectX::XMStoreVector3(&out, m_worldMatrix.r[3]);
        return out;
    }

    DirectX::XMVECTOR Transform::PositionXM() const
    {
        return m_worldMatrix.r[3];
    }

    Quaternion Transform::LocalRotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, rot_v);
        return out;
    }

    Quaternion Transform::Rotation() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_worldMatrix);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, rot_v);
        return out;
    }

    DirectX::XMVECTOR Transform::RotationXM() const
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_worldMatrix);
        return rot_v;
    }

    Vector3 Transform::Scale() const
    {
        using namespace DirectX;
        // Fill out_v with x scale, then shift in y and z values
        auto out_v = XMVectorSplatX(XMVector3Length(m_worldMatrix.r[0]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(out_v, XMVector3Length(m_worldMatrix.r[1]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0W>(out_v, XMVector3Length(m_worldMatrix.r[2]));
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    Vector3 Transform::LocalScale() const
    {
        using namespace DirectX;
        // Fill out_v with x scale, then shift in y and z values
        auto out_v = XMVectorSplatX(XMVector3Length(m_localMatrix.r[0]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(out_v, XMVector3Length(m_localMatrix.r[1]));
        out_v = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0W>(out_v, XMVector3Length(m_localMatrix.r[2]));
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    DirectX::FXMMATRIX Transform::TransformationMatrix() const
    {
        return m_worldMatrix;
    }

    Vector3 Transform::ToLocalSpace(const Vector3& vec) const
    {
        auto vec_v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
        auto out_v = DirectX::XMVector3Transform(vec_v, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    /** @note It is slighly faster to use dx identity globals instead of the vector
     *  set used in ToLocalSpace, so some code is repeated in Up/Forward/Right. */
    Vector3 Transform::Up() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR1, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    Vector3 Transform::Forward() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    Vector3 Transform::Right() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR0, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        Vector3 out;
        DirectX::XMStoreVector3(&out, out_v);
        return out;
    }

    void Transform::Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale)
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        m_localMatrix = ToScaleMatrix(scale) * ToRotMatrix(quat) * ToTransMatrix(pos);
        UpdateWorldMatrix();
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        m_localMatrix = ToScaleMatrix(scale) * ToRotMatrix(angles) * ToTransMatrix(pos);
        UpdateWorldMatrix();
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_localMatrix.r[3] = ToXMVectorHomogeneous(pos);
        UpdateWorldMatrix();
    }

    void Transform::SetRotation(const Quaternion& quat)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                        ToRotMatrix(quat) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        UpdateWorldMatrix();
    }

    void Transform::SetRotation(const Vector3& angles)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                        ToRotMatrix(angles) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        UpdateWorldMatrix();
    }

    void Transform::SetScale(const Vector3& scale)
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = ToScaleMatrix(scale) *
                        DirectX::XMMatrixRotationQuaternion(rot_v) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        UpdateWorldMatrix();
    }

    void Transform::Translate(const Vector3& translation)
    {
        const auto trans_v = ToXMVector(translation);
        auto& localPos = m_localMatrix.r[3];
        localPos = XMVectorAdd(localPos, trans_v);
        UpdateWorldMatrix();
    }

    void Transform::Translate(DirectX::FXMVECTOR translation)
    {
        auto& localPos = m_localMatrix.r[3];
        localPos = XMVectorAdd(localPos, translation);
        UpdateWorldMatrix();
    }

    void Transform::TranslateLocalSpace(const Vector3& translation)
    {
        auto trans_v = ToXMVector(translation);
        DirectX::XMVECTOR pos_v, rot_v, scl_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_worldMatrix);
        trans_v = DirectX::XMVector3Transform(trans_v, DirectX::XMMatrixRotationQuaternion(rot_v));
        trans_v = DirectX::XMVectorAndInt(trans_v, DirectX::g_XMMask3); //zero w component
        m_localMatrix.r[3] = XMVectorAdd(m_localMatrix.r[3], trans_v);
        UpdateWorldMatrix();
    }

    void Transform::Rotate(const Quaternion& quat)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= ToRotMatrix(quat);
        m_localMatrix.r[3] = pos_v;
        UpdateWorldMatrix();
    }

    void Transform::Rotate(DirectX::FXMVECTOR quaternion)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= DirectX::XMMatrixRotationQuaternion(quaternion);
        m_localMatrix.r[3] = pos_v;
        UpdateWorldMatrix();
    }

    void Transform::Rotate(const Vector3& axis, float radians)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= ToRotMatrix(axis, radians);
        m_localMatrix.r[3] = pos_v;
        UpdateWorldMatrix();
    }

    std::span<Entity> Transform::Children()
    {
        return std::span<Entity>(m_children.data(), m_children.size());
    }

    Entity Transform::Root() const
    {
        if(m_parent.Valid())
            return ActiveRegistry()->Get<Transform>(m_parent)->Root();
        
        return ParentEntity();
    }

    Entity Transform::Parent() const
    {
        return m_parent;
    }

    void Transform::SetParent(Entity parent)
    {
        auto* registry = ActiveRegistry();

        if(m_parent.Valid())
            registry->Get<Transform>(m_parent)->RemoveChild(ParentEntity());
        
        m_parent = parent;

        if(m_parent.Valid())
            registry->Get<Transform>(m_parent)->AddChild(ParentEntity());
    }

    void Transform::AddChild(Entity child)
    {
        m_children.push_back(child);
    }

    void Transform::RemoveChild(Entity child)
    {
        auto pos = std::remove_if(m_children.begin(), m_children.end(), [child](auto& c)
        {
            return child == c;
        });

        NC_ASSERT(pos != m_children.cend(), "Child does not exists");
        m_children.erase(pos, m_children.end());
    }

    void Transform::UpdateWorldMatrix()
    {
        auto* registry = ActiveRegistry();

        if(m_parent.Valid())
            m_worldMatrix = m_localMatrix * registry->Get<Transform>(m_parent)->TransformationMatrix();
        else
            m_worldMatrix = m_localMatrix;
        
        for(auto child : m_children)
            registry->Get<Transform>(child)->UpdateWorldMatrix();
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<Transform>(Transform* transform)
    {
        auto& worldMatrix = transform->m_worldMatrix;

        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, worldMatrix);
        Vector3 scl, pos;
        auto rot = Quaternion::Identity();
        DirectX::XMStoreVector3(&scl, scl_v);
        DirectX::XMStoreQuaternion(&rot, rot_v);
        DirectX::XMStoreVector3(&pos, pos_v);

        auto angles = rot.ToEulerAngles();

        ImGui::Text("Transform");
        ui::editor::xyzWidgetHeader("   ");
        auto posResult = ui::editor::xyzWidget("Pos", "transformpos", &pos.x, &pos.y, &pos.z);
        auto rotResult = ui::editor::xyzWidget("Rot", "transformrot", &angles.x, &angles.y, &angles.z, std::numbers::pi_v<float> * -2.0f, std::numbers::pi_v<float> * 2.0f);
        auto sclResult = ui::editor::xyzWidget("Scl", "transformscl", &scl.x, &scl.y, &scl.z);

        if(posResult)
            transform->SetPosition(pos);
        if(rotResult)
            transform->SetRotation(angles);
        if(sclResult)
            transform->SetScale(scl);
    }
    #endif
}