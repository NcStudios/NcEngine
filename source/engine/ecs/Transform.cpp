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
        return ToVector3(out_v);
    }

    void Transform::Set(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR rot, DirectX::FXMVECTOR scale)
    {
        m_localMatrix = XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rot);
        m_localMatrix.r[3] = XMVectorSelect(g_XMIdentityR3.v, pos, g_XMSelect1110.v);
        m_dirty = true;
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

    void Transform::RotateAround(const Vector3& point, const Vector3& axis, float radians)
    {
        using namespace DirectX;
        const auto rotationPoint = XMLoadVector3(&point);
        const auto translation = XMVectorSubtract(m_localMatrix.r[3], rotationPoint);
        const auto rotationMatrix = ToRotMatrix(axis, radians);
        auto rotatedTranslation = XMVector3TransformCoord(translation, rotationMatrix);
        rotatedTranslation = XMVectorAdd(rotatedTranslation, rotationPoint);
        m_localMatrix.r[3] = g_XMIdentityR3;
        XMMatrixMultiply(m_localMatrix, rotationMatrix);
        m_localMatrix.r[3] = rotatedTranslation;
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
