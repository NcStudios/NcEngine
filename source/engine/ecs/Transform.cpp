#include "ncengine/ecs/Transform.h"

namespace nc
{
    using namespace DirectX;

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
        m_dirty = true;
    }

    void Transform::Set(const Vector3& pos, const Vector3& angles, const Vector3& scale)
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        m_localMatrix = ToScaleMatrix(scale) * ToRotMatrix(angles) * ToTransMatrix(pos);
        m_dirty = true;
    }

    void Transform::SetPosition(const Vector3& pos)
    {
        m_localMatrix.r[3] = ToXMVectorHomogeneous(pos);
        m_dirty = true;
    }

    void Transform::SetRotation(const Quaternion& quat)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                        ToRotMatrix(quat) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        m_dirty = true;
    }

    void Transform::SetRotation(const Vector3& angles)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                        ToRotMatrix(angles) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        m_dirty = true;
    }

    void Transform::SetScale(const Vector3& scale)
    {
        NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
        m_localMatrix = ToScaleMatrix(scale) *
                        DirectX::XMMatrixRotationQuaternion(rot_v) *
                        DirectX::XMMatrixTranslationFromVector(pos_v);
        m_dirty = true;
    }

    void Transform::Translate(const Vector3& translation)
    {
        const auto trans_v = ToXMVector(translation);
        auto& localPos = m_localMatrix.r[3];
        localPos = XMVectorAdd(localPos, trans_v);
        m_dirty = true;
    }

    void Transform::Translate(DirectX::FXMVECTOR translation)
    {
        auto& localPos = m_localMatrix.r[3];
        localPos = XMVectorAdd(localPos, translation);
        m_dirty = true;
    }

    void Transform::TranslateLocalSpace(const Vector3& translation)
    {
        auto trans_v = ToXMVector(translation);
        DirectX::XMVECTOR pos_v, rot_v, scl_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_worldMatrix);
        trans_v = DirectX::XMVector3Transform(trans_v, DirectX::XMMatrixRotationQuaternion(rot_v));
        trans_v = DirectX::XMVectorAndInt(trans_v, DirectX::g_XMMask3); //zero w component
        m_localMatrix.r[3] = XMVectorAdd(m_localMatrix.r[3], trans_v);
        m_dirty = true;
    }

    void Transform::Rotate(const Quaternion& quat)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= ToRotMatrix(quat);
        m_localMatrix.r[3] = pos_v;
        m_dirty = true;
    }

    void Transform::Rotate(DirectX::FXMVECTOR quaternion)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= DirectX::XMMatrixRotationQuaternion(quaternion);
        m_localMatrix.r[3] = pos_v;
        m_dirty = true;
    }

    void Transform::Rotate(const Vector3& axis, float radians)
    {
        auto pos_v = m_localMatrix.r[3];
        m_localMatrix.r[3] = DirectX::g_XMIdentityR3;
        m_localMatrix *= ToRotMatrix(axis, radians);
        m_localMatrix.r[3] = pos_v;
        m_dirty = true;
    }

    void Transform::LookAt(const Vector3& target)
    {
        const auto selfToTarget = target - Position();
        const auto forward = Normalize(selfToTarget);
        const auto cosTheta = Dot(Vector3::Front(), forward);
        const auto angle = std::acos(cosTheta);
        const auto axis = Normalize(CrossProduct(Vector3::Front(), forward));
        SetRotation(Quaternion::FromAxisAngle(axis, angle));
    }
}
