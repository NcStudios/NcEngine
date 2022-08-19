#pragma once

#include "Component.h"
#include "math/Vector.h"
#include "math/Quaternion.h"
#include "DirectXMath.h"

#include <span>
#include <vector>

namespace nc
{
/** @brief Component with translation, rotation, and scale properties.
 * 
 *  @note Transforms are automatically added and removed with their parent
 *        Entity. They do not need to be explicity created.
 * 
 *  A Transform's initial values can be set in EntityInfo when adding an Entity.
 * 
 *  Data is stored for local and world space, but only local space values may be
 *  directly modified. World space calculations are done internally based on the
 *  Transform/Entity hierarchy.
*/
class Transform final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(Transform)

    public:
        Transform(Entity entity, const Vector3& pos, const Quaternion& rot, const Vector3& scale, Entity parent);
        Transform(Transform&&) = default;
        Transform& operator=(Transform&&) = default;
        ~Transform() = default;
        Transform(const Transform&) = delete;
        Transform& operator=(const Transform&) = delete;

        /** @brief Get world space position */
        auto Position() const -> Vector3;

        /** @brief Get world space position as XMVECTOR */
        auto PositionXM() const -> DirectX::XMVECTOR;

        /** @brief Get local space position */
        auto LocalPosition() const -> Vector3;

        /** @brief Get world space rotation */
        auto Rotation() const -> Quaternion;

        /** @brief Get world space rotation as XMVECTOR */
        auto RotationXM() const -> DirectX::XMVECTOR;

        /** @brief Get local space rotation */
        auto LocalRotation() const -> Quaternion;

        /** @brief Get world space scale */
        auto Scale() const -> Vector3;

        /** @brief Get local space scale */
        auto LocalScale() const -> Vector3;

        /** @brief Get world space matrix */
        auto TransformationMatrix() const -> DirectX::FXMMATRIX;

        /** @brief Get local space matrix */
        auto ToLocalSpace(const Vector3& vec) const -> Vector3;

        /** @brief Get the up axis of the transform */
        auto Up() const -> Vector3;

        /** @brief Get the forward axis of the transform */
        auto Forward() const -> Vector3;

        /** @brief Get the right axis of the transform */
        auto Right() const -> Vector3;

        /** @brief Set all local values [Quaternion] */
        void Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale);

        /** @brief Set all local values [Angles] */
        void Set(const Vector3& pos, const Vector3& angles, const Vector3& scale);

        /** @brief Set local position */
        void SetPosition(const Vector3& pos);

        /** @brief Set local rotation [Quaternion] */
        void SetRotation(const Quaternion& quat);

        /** @brief Set local rotation [Angles] */
        void SetRotation(const Vector3& angles);

        /** @brief Set local scale */
        void SetScale(const Vector3& scale);

        /** @brief Add to local position */
        void Translate(const Vector3& vec);

        /** @brief Add to local position [XMVECTOR] */
        void Translate(DirectX::FXMVECTOR translation);

        /** @brief Rotate a vector into local space and add it to local position */
        void TranslateLocalSpace(const Vector3& vec);

        /** @brief Apply rotation quaternion to local rotation */
        void Rotate(const Quaternion& quat);

        /** @brief Apply a rotation quaternion to local rotation [XMVECTOR] */
        void Rotate(DirectX::FXMVECTOR quaternion);

        /** @brief Apply a rotation about an axis to local rotation */
        void Rotate(const Vector3& axis, float radians);

        /** @brief Get all immediate children of this transform */
        auto Children() -> std::span<Entity>;

        /** @brief Get the root node relative to this transform */
        auto Root() const -> Entity;

        /** @brief Get the immediate parent of this transform - may be nullptr */
        auto Parent() const -> Entity;

        /** @brief Make this transform the child of another, or pass nullptr to detach from existing parent */
        void SetParent(Entity parent);

    private:
        void AddChild(Entity child);
        void RemoveChild(Entity child);
        void UpdateWorldMatrix();

        DirectX::XMMATRIX m_localMatrix;
        DirectX::XMMATRIX m_worldMatrix;
        Entity m_parent;
        std::vector<Entity> m_children;

        #ifdef NC_EDITOR_ENABLED
        friend void ComponentGuiElement<Transform>(Transform*);
        #endif
};

#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<Transform>(Transform* transform);
#endif
} //end namespace nc
