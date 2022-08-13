#pragma once

#include "Component.h"
#include "math/Vector.h"
#include "math/Quaternion.h"
#include "DirectXMath.h"

#include <span>
#include <vector>

namespace nc
{
class Transform final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(Transform)

    public:
        Transform(Entity entity, const Vector3& pos, const Quaternion& rot, const Vector3& scale, Entity parent);
        ~Transform() = default;
        Transform(const Transform&) = delete;
        Transform(Transform&&) = default;
        Transform& operator=(const Transform&) = delete;
        Transform& operator=(Transform&&) = default;

        auto Position() const -> Vector3;                        // Get world space position
        auto PositionXM() const -> DirectX::XMVECTOR;            // Get world space position as XMVECTOR
        auto LocalPosition() const -> Vector3;                   // Get position relative to parent
        auto Rotation() const -> Quaternion;                     // Get world space rotation
        auto RotationXM() const -> DirectX::XMVECTOR;            // Get world space rotation as XMVECTOR
        auto LocalRotation() const -> Quaternion;                // Get rotation relative to parent
        auto Scale() const -> Vector3;                           // Get world space scale
        auto LocalScale() const -> Vector3;                      // Get scale relative to parent
        auto TransformationMatrix() const -> DirectX::FXMMATRIX; // Get world space transformation matrix
        auto ToLocalSpace(const Vector3& vec) const -> Vector3;  // Transform a vector by this transforms's world space matrix
        auto Up() const -> Vector3;                              // Get the up axis of the transform
        auto Forward() const -> Vector3;                         // Get the forward axis of the transform
        auto Right() const -> Vector3;                           // Get the right axis of the transform

        // All modification methods operate on the local matrix
        void Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale);
        void Set(const Vector3& pos, const Vector3& angles, const Vector3& scale);
        void SetPosition(const Vector3& pos);
        void SetRotation(const Quaternion& quat);
        void SetRotation(const Vector3& angles);
        void SetScale(const Vector3& scale);
        void Translate(Vector3 vec);
        void Translate(DirectX::FXMVECTOR translation);
        void TranslateLocalSpace(Vector3 vec);
        void Rotate(const Quaternion& quat);
        void Rotate(DirectX::FXMVECTOR quaternion);
        void Rotate(Vector3 axis, float radians);

        // Hierarchy methods
        auto Children() -> std::span<Entity>; // Get all immediate children of this transform
        auto Root() const -> Entity;          // Get the root node relative to this transform
        auto Parent() const -> Entity;        // Get the immediate parent of this transform - may be nullptr
        void SetParent(Entity parent);        // Make this transform the child of another - nullptr will detach from existing parent

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
