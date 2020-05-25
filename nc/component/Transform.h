#pragma once
#include "Common.h"
#include "Component.h"
#include "Vector3.h"

//namespace nc::utils::editor { class EditorManager; }
namespace DirectX { struct XMMATRIX; }


namespace nc
{
    class Transform : public Component
    {
        public:
            Transform(ComponentHandle handle, EntityView parentView) noexcept;

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            #endif

            inline Vector3 GetPosition() const noexcept { return m_position;     }
            inline Vector3 GetRotation() const noexcept { return m_rotation;     }
            inline Vector3 GetScale()    const noexcept { return m_scale;        }
            inline float   Pitch()       const noexcept { return m_rotation.m_x; }
            inline float   Yaw()         const noexcept { return m_rotation.m_y; }
            inline float   Roll()        const noexcept { return m_rotation.m_z; }

            DirectX::XMMATRIX GetMatrixXM();
            DirectX::XMMATRIX CamGetMatrix();

            inline void Set(const Vector3& pos, const Vector3& rot, const Vector3& scale) noexcept { m_position = pos; m_rotation = rot; m_scale = scale; }
            inline void SetPosition(const Vector3& pos)                                   noexcept { m_position = pos;   }
            inline void SetRotation(const Vector3& rot)                                   noexcept { m_rotation = rot;   }
            inline void SetScale(const Vector3& scale)                                    noexcept { m_scale    = scale; }

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