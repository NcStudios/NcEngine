#pragma once

#include "nc_engine.h"
#include "Input.h"
#include "MainCamera.h"

namespace nc::editor
{
    /** Control camera pan, look, and zoom with right mouse button, middle mouse button, and mouse wheel. */
    class SceneNavigationCamera : public Camera
    {
        public:
            SceneNavigationCamera(Entity entity, float panDamp, float lookDamp, float zoomDamp);
            void Run(Entity self, Registry* registry, float dt);

        private:
            Vector2 m_panPivot = Vector2::Zero();
            Vector2 m_lookPivot = Vector2::Zero();
            float m_zoom = 0.0f;
            float m_panDampen = 0.25f;
            float m_lookDampen = 0.005f;
            float m_zoomDampen = 1.4f;

            void Pan(float dt, Transform* transform);
            void Look(float dt, Transform* transform);
            void Zoom(float dt, Transform* transform);
    };

    inline SceneNavigationCamera::SceneNavigationCamera(Entity entity, float panDamp, float lookDamp, float zoomDamp)
        : Camera{entity},
          m_panDampen{panDamp},
          m_lookDampen{lookDamp},
          m_zoomDampen{zoomDamp}
    {
    }

    inline void SceneNavigationCamera::Run(Entity, Registry* registry, float dt)
    {
        auto* transform = registry->Get<Transform>(ParentEntity());
        Pan(dt, transform);
        Look(dt, transform);
        Zoom(dt, transform);
    }

    inline void SceneNavigationCamera::Pan(float dt, Transform* transform)
    {
        if(input::GetKeyDown(input::KeyCode::MiddleButton))
            m_panPivot = input::MousePos();
        else if(input::GetKeyUp(input::KeyCode::MiddleButton))
            m_panPivot = Vector2::Zero();
        
        if(m_panPivot != Vector2::Zero())
        {
            auto mouseDelta = input::MousePos() - m_panPivot;
            auto [horizontalPan, verticalPan] = mouseDelta * m_panDampen * dt;
            transform->TranslateLocalSpace(Vector3{-1.0f * horizontalPan, verticalPan, 0.0f});
        }
    }

    inline void SceneNavigationCamera::Look(float dt, Transform* transform)
    {
        if(input::GetKeyDown(input::KeyCode::RightButton))
            m_lookPivot = input::MousePos();
        else if(input::GetKeyUp(input::KeyCode::RightButton))
            m_lookPivot = Vector2::Zero();

        if(m_lookPivot != Vector2::Zero())
        {
            auto mouseDelta = input::MousePos() - m_lookPivot;
            auto [horizontalLook, verticalLook] = mouseDelta * m_lookDampen * dt;
            transform->Rotate(Vector3::Up(), horizontalLook);
            transform->Rotate(transform->Right(), verticalLook);
        }
    }

    inline void SceneNavigationCamera::Zoom(float dt, Transform* transform)
    {
        m_zoom = math::Lerp(m_zoom, (float)input::MouseWheel() * m_zoomDampen, 0.1f);
        if(math::FloatEqual(m_zoom, 0.0f))
            m_zoom = 0.0f;
        transform->TranslateLocalSpace(Vector3{0.0f, 0.0f, m_zoom * dt});
    }
}