#pragma once
#include "component/Component.h"
#include "math/Vector3.h"

namespace DirectX { struct XMMATRIX; }

namespace nc
{
    enum class Space : uint8_t
    {
        World,
        Local
    };

    class Transform final : public Component
    {
        public:
            Transform(ComponentHandle handle, EntityHandle parentHandle, const Vector3& pos, const Vector3& rot, const Vector3& scl) noexcept;
            Transform(const Transform&) = delete;
            Transform(Transform&&) = delete;
            Transform& operator=(const Transform&) = delete;
            Transform& operator=(Transform&&) = delete;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            inline Vector3 GetPosition() const noexcept { return m_position; }
            inline Vector3 GetRotation() const noexcept { return m_rotation; }
            inline Vector3 GetScale() const noexcept { return m_scale; }
            inline float PosX() const noexcept { return m_position.x; }
            inline float PosY() const noexcept { return m_position.y; }
            inline float PosZ() const noexcept { return m_position.z; }
            inline float SclX() const noexcept { return m_rotation.x; }
            inline float SclY() const noexcept { return m_rotation.y; }
            inline float SclZ() const noexcept { return m_rotation.z; }
            inline float RotX() const noexcept { return m_rotation.x; }
            inline float RotY() const noexcept { return m_rotation.y; }
            inline float RotZ() const noexcept { return m_rotation.z; }
            /* Rotation Alternatives */
            inline float Pitch() const noexcept { return m_rotation.x; }
            inline float Yaw() const noexcept { return m_rotation.y; }
            inline float Roll() const noexcept { return m_rotation.z; }

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

            void Translate(Vector3 vec, Space space = Space::Local) noexcept;
            void Rotate(float xAngle, float yAngle, float zAngle, float speed) noexcept;
            void RotateClamped(float xAngle, float yAngle, float zAngle, float speed, float min, float max) noexcept;

        private:
            Vector3 m_position;
            Vector3 m_rotation;
            Vector3 m_scale;
    };
} //end namespace nc