#pragma once
#include "component/Component.h"
#include "math/Vector3.h"

namespace DirectX { struct XMMATRIX; }

namespace nc
{
    class Transform : public Component
    {
        public:
            Transform() noexcept;
            Transform(const Vector3& pos, const Vector3& rot, const Vector3& scl) noexcept;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            inline Vector3 GetPosition() const noexcept { return m_position; }
            inline Vector3 GetRotation() const noexcept { return m_rotation; }
            inline Vector3 GetScale() const noexcept { return m_scale; }
            inline float PosX() const noexcept { return m_position.X(); }
            inline float PosY() const noexcept { return m_position.Y(); }
            inline float PosZ() const noexcept { return m_position.Z(); }
            inline float SclX() const noexcept { return m_rotation.X(); }
            inline float SclY() const noexcept { return m_rotation.Y(); }
            inline float SclZ() const noexcept { return m_rotation.Z(); }
            inline float RotX() const noexcept { return m_rotation.X(); }
            inline float RotY() const noexcept { return m_rotation.Y(); }
            inline float RotZ() const noexcept { return m_rotation.Z(); }
            /* Rotation Alternatives */
            inline float Pitch() const noexcept { return m_rotation.m_x; }
            inline float Yaw() const noexcept { return m_rotation.m_y; }
            inline float Roll() const noexcept { return m_rotation.m_z; }

            DirectX::XMMATRIX GetMatrixXM() const;
            DirectX::XMMATRIX CamGetMatrix() const;

            inline void Set(const Vector3& pos, const Vector3& rot, const Vector3& scale) noexcept 
            {
                m_position = pos; m_rotation = rot; m_scale = scale;
            }
            inline void SetPosition(const Vector3& pos) noexcept
            {
                m_position = pos;
            }
            inline void SetRotation(const Vector3& rot) noexcept
            {
                m_rotation = rot;
            }
            inline void SetScale(const Vector3& scale) noexcept
            {
                m_scale = scale;
            }

            void Translate(const Vector3& vec) noexcept;
            void CamTranslate(Vector3& translation, float factor) noexcept;
            void Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept;
            void RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept;
            //void MoveToward()

        private:
            Vector3 m_position;
            Vector3 m_rotation;
            Vector3 m_scale;
    };
} //end namespace nc