#include "NcPhysics.h"
#include "physics/PhysicsSystem.h"
#include "window/Window.h"
#include "ecs/Ecs.h"
#include "camera/MainCamera.h"
#include "ecs/RenderingSystem.h"
#include "graphics/Graphics.h"

namespace nc::physics
{
    void NcRegisterClickable(IClickable* clickable)
    {
        PhysicsSystem::RegisterClickable(clickable);
    }

    void NcUnregisterClickable(IClickable* clickable)
    {
        PhysicsSystem::UnregisterClickable(clickable);
    }

    IClickable* NcRaycastToClickables(LayerMask mask)
    {
        return PhysicsSystem::RaycastToClickables
        (
            camera::MainCamera::GetTransform()->CamGetMatrix(),
            ecs::ECS::GetRenderingSystem()->GetGraphics()->GetProjection(),
            Window::Instance->GetWindowDimensions(),
            mask
        );
    }
}