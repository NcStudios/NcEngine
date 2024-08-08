/**
 * @file Transform.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Component.h"

#include "ncengine/utility/MatrixUtilities.h"

#include <span>
#include <vector>

namespace nc
{
namespace ecs
{
class EcsModule;
}

/** @brief Component with translation, rotation, and scale properties.
 * 
 *  @note Transforms are automatically added and removed with their parent
 *        Entity. They do not need to be explicity created.
 * 
 *  A Transform's initial values can be set in EntityInfo when adding an Entity.
 * 
 *  Data is stored for local and world space, but only local space values may be
 *  directly modified. World space calculations are done internally based on the
 *  Entity's Hierarchy component.
*/
class Transform final : public ComponentBase
{
    public:
        Transform(Entity entity, const Vector3& pos, const Quaternion& rot, const Vector3& scale)
            : ComponentBase(entity),
              m_localMatrix{ComposeMatrix(scale, rot, pos)},
              m_worldMatrix{m_localMatrix}
        {
            NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        }

        Transform(Entity entity,
                  const Vector3& pos,
                  const Quaternion& rot,
                  const Vector3& scale,
                  DirectX::FXMMATRIX parentTransform)
            : ComponentBase(entity),
              m_localMatrix{ComposeMatrix(scale, rot, pos)},
              m_worldMatrix{m_localMatrix * parentTransform}
        {
            NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
        }

        Transform(Transform&&) = default;
        Transform& operator=(Transform&&) = default;
        ~Transform() = default;
        Transform(const Transform&) = delete;
        Transform& operator=(const Transform&) = delete;

        /** @brief Get world space position */
        auto Position() const noexcept -> Vector3 { return ToVector3(m_worldMatrix.r[3]); }

        /** @brief Get world space position as XMVECTOR */
        auto PositionXM() const noexcept -> DirectX::FXMVECTOR { return m_worldMatrix.r[3]; }

        /** @brief Get local space position */
        auto LocalPosition() const noexcept -> Vector3 { return ToVector3(m_localMatrix.r[3]); }

        /** @brief Get world space rotation */
        auto Rotation() const noexcept -> Quaternion { return ToQuaternion(RotationXM()); }

        /** @brief Get world space rotation as XMVECTOR */
        auto RotationXM() const noexcept -> DirectX::XMVECTOR { return DecomposeRotation(m_worldMatrix); }

        /** @brief Get local space rotation */
        auto LocalRotation() const noexcept -> Quaternion { return ToQuaternion(DecomposeRotation(m_localMatrix)); }

        /** @brief Get world space scale */
        auto Scale() const noexcept -> Vector3 { return ToVector3(DecomposeScale(m_worldMatrix)); }

        /** @brief Get local space scale */
        auto LocalScale() const noexcept -> Vector3 { return ToVector3(DecomposeScale(m_localMatrix)); }

        /** @brief Get world space matrix */
        auto TransformationMatrix() const noexcept -> DirectX::FXMMATRIX { return m_worldMatrix; }

        /** @brief Get local space matrix */
        auto LocalTransformationMatrix() const noexcept -> DirectX::FXMMATRIX { return m_localMatrix; }

        /** @brief Get local space matrix */
        auto ToLocalSpace(const Vector3& vec) const -> Vector3;

        /** @brief Get the up axis of the transform */
        auto Up() const noexcept -> Vector3 { return ToVector3(UpXM()); }

        /** @brief Get the up axis of the transform as an XMVECTOR */
        auto UpXM() const noexcept -> DirectX::XMVECTOR
        {
            auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR1, m_worldMatrix);
            out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
            return DirectX::XMVector3Normalize(out_v);
        }

        /** @brief Get the forward axis of the transform */
        auto Forward() const noexcept -> Vector3 { return ToVector3(ForwardXM()); }

        /** @brief Get the forward axis of the transform as an XMVECTOR */
        auto ForwardXM() const noexcept -> DirectX::XMVECTOR
        {
            auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m_worldMatrix);
            out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
            return DirectX::XMVector3Normalize(out_v);
        }

        /** @brief Get the right axis of the transform */
        auto Right() const noexcept -> Vector3 { return ToVector3(RightXM()); }

        /** @brief Get the right axis of the transform as an XMVECTOR */
        auto RightXM() const noexcept -> DirectX::XMVECTOR
        {
            auto out_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR0, m_worldMatrix);
            out_v = DirectX::XMVectorSubtract(out_v, m_worldMatrix.r[3]);
            return DirectX::XMVector3Normalize(out_v);
        }

        /** @brief Set all local values [Matrix] */
        void Set(DirectX::FXMMATRIX matrix)
        {
            m_localMatrix = matrix;
            m_dirty = true;
        }

        /** @brief Set all local values [XM] */
        void Set(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR rot, DirectX::FXMVECTOR scale);

        /** @brief Set all local values [Quaternion] */
        void Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale);

        /** @brief Set all local values [Angles] */
        void Set(const Vector3& pos, const Vector3& angles, const Vector3& scale);

        /** @brief Set local position */
        void SetPosition(const Vector3& pos)
        {
            SetPosition(ToXMVectorHomogeneous(pos));
        }

        /** @brief Set local position [XM] */
        void SetPosition(DirectX::FXMVECTOR pos)
        {
            m_localMatrix.r[3] = pos;
            m_dirty = true;
        }

        /** @brief Set local rotation [Quaternion] */
        void SetRotation(const Quaternion& quaternion)
        {
            SetRotation(ToRotMatrix(quaternion));
        }

        /** @brief Set local rotation [Angles] */
        void SetRotation(const Vector3& angles)
        {
            SetRotation(ToRotMatrix(angles));
        }

        /** @brief Set local rotation [XM Quaternion] */
        void SetRotation(DirectX::FXMVECTOR quaternion)
        {
            SetRotation(DirectX::XMMatrixRotationQuaternion(quaternion));
        }

        /** @brief Set local rotation [XM Matrix] */
        void SetRotation(DirectX::FXMMATRIX rotation)
        {
            DirectX::XMVECTOR scl_v, rot_v, pos_v;
            DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
            m_localMatrix = DirectX::XMMatrixScalingFromVector(scl_v) *
                            rotation *
                            DirectX::XMMatrixTranslationFromVector(pos_v);
            m_dirty = true;
        }

        /** @brief Set local scale */
        void SetScale(const Vector3& scale)
        {
            NC_ASSERT(!HasAnyZeroElement(scale), "Invalid scale(elements cannot be 0)");
            DirectX::XMVECTOR scl_v, rot_v, pos_v;
            DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, m_localMatrix);
            m_localMatrix = ToScaleMatrix(scale) *
                            DirectX::XMMatrixRotationQuaternion(rot_v) *
                            DirectX::XMMatrixTranslationFromVector(pos_v);
            m_dirty = true;
        }

        /** @brief Add to local position */
        void Translate(const Vector3& translation)
        {
            Translate(ToXMVector(translation));
        }

        /** @brief Add to local position [XMVECTOR] */
        void Translate(DirectX::FXMVECTOR translation)
        {
            m_localMatrix.r[3] = DirectX::XMVectorAdd(m_localMatrix.r[3], translation);
            m_dirty = true;
        }

        /** @brief Rotate a vector into local space and add it to local position */
        void TranslateLocalSpace(const Vector3& vec);

        /** @brief Apply rotation quaternion to local rotation */
        void Rotate(const Quaternion& quat)
        {
            Rotate(ToRotMatrix(quat));
        }

        /** @brief Apply a rotation about an axis to local rotation */
        void Rotate(const Vector3& axis, float radians)
        {
            Rotate(ToRotMatrix(axis, radians));
        }

        /** @brief Apply a rotation quaternion to local rotation [XMVECTOR] */
        void Rotate(DirectX::FXMVECTOR quaternion)
        {
            Rotate(DirectX::XMMatrixRotationQuaternion(quaternion));
        }

        /** @brief Apply a rotation quaternion to local rotation [XMMATRIX] */
        void Rotate(DirectX::FXMMATRIX rotation)
        {
            const auto pos_v = std::exchange(m_localMatrix.r[3], DirectX::g_XMIdentityR3);
            m_localMatrix *= rotation;
            m_localMatrix.r[3] = pos_v;
            m_dirty = true;
        }

        /** @brief Apply a rotation about an axis passing through a point. */
        void RotateAround(const Vector3& point, const Vector3& axis, float radians);

        /** @brief Rotate to point towards a target position. */
        void LookAt(const Vector3& target);

    private:
        friend class ecs::EcsModule;
        bool m_dirty = false;
        DirectX::XMMATRIX m_localMatrix;
        DirectX::XMMATRIX m_worldMatrix;

        auto IsDirty() const noexcept
        {
            return m_dirty;
        }

        void UpdateWorldMatrix()
        {
            m_dirty = false;
            m_worldMatrix = m_localMatrix;
        }

        void UpdateWorldMatrix(DirectX::FXMMATRIX parentMatrix)
        {
            m_dirty = false;
            m_worldMatrix = m_localMatrix * parentMatrix;
        }
};
} //end namespace nc
