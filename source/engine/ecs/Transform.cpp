#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/Registry.h"

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

    Vector3 Transform::ToLocalSpace(const Vector3& vec) const
    {
        auto vec_v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f);
        auto out_v = DirectX::XMVector3Transform(vec_v, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        return ToVector3(out_v);
    }

    /** @note It is slighly faster to use dx identity globals instead of the vector
     *  set used in ToLocalSpace, so some code is repeated in Up/Forward/Right. */
    Vector3 Transform::Up() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR1, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        return ToVector3(out_v);
    }

    Vector3 Transform::Forward() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        return ToVector3(out_v);
    }

    Vector3 Transform::Right() const
    {
        auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR0, m_worldMatrix);
        out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
        out_v = DirectX::XMVector3Normalize(out_v);
        return ToVector3(out_v);
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

    Entity Transform::Root() const
    {
        if(m_parent.Valid())
            return ActiveRegistry()->Get<Transform>(m_parent)->Root();
        
        return ParentEntity();
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
}
