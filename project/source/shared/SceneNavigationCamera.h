#pragma once

#include "Ecs.h"
#include "Input.h"

namespace nc::sample
{
    /** Control camera pan, look, and zoom with right mouse button, middle mouse button, and mouse wheel. */
    class SceneNavigationCamera : public Camera
    {
        public:
            SceneNavigationCamera(EntityHandle handle);
            void FrameUpdate(float dt) override;

        private:
            Transform* m_transform;
            Vector2 m_panPivot = Vector2::Zero();
            Vector2 m_lookPivot = Vector2::Zero();
            float m_zoom = 0.0f;
            float m_panDampen = 0.25f;
            float m_lookDampen = 0.01f;
            float m_zoomDampen = 1.2f;

            void Pan(float dt);
            void Look(float dt);
            void Zoom(float dt);
    };

    inline SceneNavigationCamera::SceneNavigationCamera(EntityHandle handle)
        : Camera{handle},
          m_transform{GetComponent<Transform>(handle)}
    {
    }

    inline void SceneNavigationCamera::FrameUpdate(float dt)
    {
        Pan(dt);
        Look(dt);
        Zoom(dt);
    }

    inline void SceneNavigationCamera::Pan(float dt)
    {
        if(input::GetKeyDown(input::KeyCode::MiddleButton))
            m_panPivot = input::MousePos();
        else if(input::GetKeyUp(input::KeyCode::MiddleButton))
            m_panPivot = Vector2::Zero();
        
        if(m_panPivot != Vector2::Zero())
        {
            auto mouseDelta = input::MousePos() - m_panPivot;
            auto [horizontalPan, verticalPan] = mouseDelta * m_panDampen * dt;
            m_transform->Translate(Vector3{-1.0f * horizontalPan, verticalPan, 0.0f}, Space::Local);
        }
    }

    inline void SceneNavigationCamera::Look(float dt)
    {
        if(input::GetKeyDown(input::KeyCode::RightButton))
            m_lookPivot = input::MousePos();
        else if(input::GetKeyUp(input::KeyCode::RightButton))
            m_lookPivot = Vector2::Zero();

        if(m_lookPivot != Vector2::Zero())
        {
            auto mouseDelta = input::MousePos() - m_lookPivot;
            auto [horizontalLook, verticalLook] = mouseDelta * m_lookDampen * dt;
            /** @note need local space vectors here - fix in next pr */
            m_transform->Rotate(Vector3::Up(), horizontalLook);
            m_transform->Rotate(Vector3::Right(), verticalLook);
        }
    }

    inline void SceneNavigationCamera::Zoom(float dt)
    {
        m_zoom = math::Lerp(m_zoom, (float)input::MouseWheel() * m_zoomDampen, 0.1f);
        if(math::FloatEqual(m_zoom, 0.0f))
            m_zoom = 0.0f;
        m_transform->Translate(Vector3{0.0f, 0.0f, m_zoom * dt}, Space::Local);
    }
}