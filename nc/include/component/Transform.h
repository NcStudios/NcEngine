#pragma once

#include "component/Component.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "directx/math/DirectXMath.h"

#include <span>
#include <vector>

namespace nc
{
    class Transform final : public ComponentBase
    {
        public:
            Transform(EntityHandle handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale, Transform* parent);
            ~Transform() = default;
            Transform(const Transform&) = delete;
            Transform(Transform&&) = delete;
            Transform& operator=(const Transform&) = delete;
            Transform& operator=(Transform&&) = delete;

            Vector3 GetPosition() const;                        // Get world space position
            Vector3 GetLocalPosition() const;                   // Get position relative to parent
            Quaternion GetRotation() const;                     // Get world space rotation
            Quaternion GetLocalRotation() const;                // Get rotation relative to parent
            Vector3 GetScale() const;                           // Get world space scale
            Vector3 GetLocalScale() const;                      // Get scale relative to parent
            DirectX::FXMMATRIX GetTransformationMatrix() const; // Get world space transformation matrix
            Vector3 ToLocalSpace(const Vector3& vec) const;     // Transform a vector by this transforms's world space matrix
            Vector3 Up() const;                                 // Get the up axis of the transform
            Vector3 Forward() const;                            // Get the forward axis of the transform
            Vector3 Right() const;                              // Get the right axis of the transform

            // All modification methods operate on the local matrix
            void Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale);
            void Set(const Vector3& pos, const Vector3& angles, const Vector3& scale);
            void SetPosition(const Vector3& pos);
            void SetRotation(const Quaternion& quat);
            void SetRotation(const Vector3& angles);
            void SetScale(const Vector3& scale);
            void Translate(Vector3 vec);
            void TranslateLocalSpace(Vector3 vec);
            void Rotate(const Quaternion& quat);
            void Rotate(Vector3 axis, float radians);

            // Hierarchy methods
            std::span<Transform*> GetChildren(); // Get all immediate children of this transform
            Transform* GetRoot();                // Get the root node relative to this transform
            Transform* GetParent() const;        // Get the immediate parent of this transform - may be nullptr
            void SetParent(Transform* parent);   // Make this transform the child of another - nullptr will detach from existing parent

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
            void AddChild(Transform* child);
            void RemoveChild(Transform* child);
            void UpdateWorldMatrix();

            DirectX::XMMATRIX m_localMatrix;
            DirectX::XMMATRIX m_worldMatrix;
            Transform* m_parent;
            std::vector<Transform*> m_children;
    };
} //end namespace nc