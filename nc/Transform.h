#pragma once
#include "Common.h"
#include "Component.h"
#include "NCVector.h"

//#include "DirectXMath.h"

namespace DirectX { struct XMMATRIX; }

namespace nc
{
    class Transform : public Component
    {
        public:
            Transform(ComponentHandle handle, EntityView parentView) noexcept;

            inline Vector3 GetPosition() const noexcept;
            inline Vector3 GetRotation() const noexcept;
            inline Vector3 GetScale()    const noexcept;

            DirectX::XMMATRIX GetMatrixXM();
            DirectX::XMMATRIX CamGetMatrix();
            
            void Set(const Vector3& pos, const Vector3& rot, const Vector3& scale) noexcept;
            inline void SetPosition(const Vector3& pos) noexcept;
            inline void SetRotation(const Vector3& rot) noexcept;
            inline void SetScale(const Vector3& scale)  noexcept;

            void Translate(const Vector3& vec) noexcept;
            void CamTranslate(Vector3& translation, float factor) noexcept;
            void Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept;
            void RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept;

        private:
            Vector3 m_position;
            Vector3 m_rotation;
            Vector3 m_scale;
    };
} //end namespace nc