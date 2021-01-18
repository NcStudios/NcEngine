#pragma once
#include "component/Component.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "directx/math/DirectXMath.h"

namespace DirectX { struct XMMATRIX; }

namespace nc
{
    enum class Space : uint8_t
    {
        World,
        Local
    };

    struct DecomposedMatrix
    {
        DirectX::XMFLOAT4 rotation;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 scale;
    };

    class Transform final : public Component
    {
        public:
            Transform(EntityHandle handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale) noexcept;
            Transform(const Transform&) = delete;
            Transform(Transform&&) = delete;
            Transform& operator=(const Transform&) = delete;
            Transform& operator=(Transform&&) = delete;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            Vector3 GetPosition() const;
            Quaternion GetRotation() const;
            Vector3 GetScale() const;

            DirectX::XMMATRIX GetTransformationMatrix() const;
            DirectX::XMMATRIX GetTransformationMatrixEx(Vector3 additionalScale) const;
            DirectX::XMMATRIX GetViewMatrix() const;

            void Set(const Vector3& pos, const Quaternion& quat, const Vector3& scale);
            void Set(const Vector3& pos, const Vector3& angles, const Vector3& scale);
            void SetPosition(const Vector3& pos);
            void SetRotation(const Quaternion& quat);
            void SetRotation(const Vector3& angles);
            void SetScale(const Vector3& scale);

            void Translate(Vector3 vec, Space space = Space::Local);
            void Rotate(const Quaternion& quat);
            void Rotate(Vector3 axis, float radians);

        private:
            DirectX::XMMATRIX m_matrix;
    };
} //end namespace nc