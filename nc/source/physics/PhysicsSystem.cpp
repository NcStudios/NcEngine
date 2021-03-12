#include "PhysicsSystem.h"
#include "Physics.h"
#include "debug/Utils.h"
#include "Input.h"
#include "Window.h"
#ifdef USE_VULKAN
    #include "graphics/Graphics2.h"
#else
    #include "graphics/Graphics.h"
#endif
#include "MainCamera.h"
#include "Ecs.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace
{
    const auto FLOAT_MAX = std::numeric_limits<float>::max();
    nc::physics::PhysicsSystem* impl = nullptr;

    // temp until we can load this from scenes or something
    constexpr float WorldspaceExtent = 1000.0f;
}

namespace nc::physics
{
    /* Api Implementation */
    void RegisterClickable(IClickable* clickable)
    {
        IF_THROW(!impl, "physics::RegisterClickable - impl is not set");
        impl->RegisterClickable(clickable);
    }

    void UnregisterClickable(IClickable* clickable) noexcept
    {
        impl->UnregisterClickable(clickable);
    }

    IClickable* RaycastToClickables(LayerMask mask)
    {
        IF_THROW(!impl, "physics::RaycastToClickables - impl is not set");
        return impl->RaycastToClickables(mask);
    }

    #ifdef USE_VULKAN
    /* Physics System */
    PhysicsSystem::PhysicsSystem(graphics::Graphics2* graphics2)
        : m_clickableComponents{},
          m_graphics2{ graphics2 }
    {
        impl = this;
    }
    #else
    /* Physics System */
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics, job::ThreadPool* threadPool)
        : m_collisionSystem{WorldspaceExtent, threadPool},
          m_clickableComponents{},
          m_graphics{ graphics }
    {
        impl = this;
    }
    #endif

    void PhysicsSystem::ClearState()
    {
        m_clickableComponents.clear();
        m_collisionSystem.ClearState();
    }

    void PhysicsSystem::DoPhysicsStep()
    {
        m_collisionSystem.DoCollisionStep();
    }

    void PhysicsSystem::RegisterClickable(IClickable* toAdd)
    {
        m_clickableComponents.push_back(toAdd);
    }

    void PhysicsSystem::UnregisterClickable(IClickable* toRemove) noexcept
    {
        auto pos = std::find(m_clickableComponents.begin(), m_clickableComponents.end(), toRemove);
        if(pos != m_clickableComponents.end())
        {
            *pos = m_clickableComponents.back();
            m_clickableComponents.pop_back();
        }
    }

    IClickable* PhysicsSystem::RaycastToClickables(LayerMask mask)
    {
        auto unit = Normalize(Vector3::Splat(1.0f));
        auto unit_v = DirectX::XMLoadVector3(&unit);
        auto viewMatrix = camera::GetMainCameraTransform()->GetViewMatrix();
        unit_v = DirectX::XMVector3Transform(unit_v, viewMatrix);
        DirectX::XMStoreVector3(&unit, unit_v);

        auto [screenWidth, screenHeight] = window::GetDimensions();
        
        #ifdef USE_VULKAN
        auto projectionMatrix = m_graphics2->GetProjectionMatrix();
        #else
        auto projectionMatrix = m_graphics->GetProjectionMatrix();
        #endif

        auto worldMatrix = DirectX::XMMatrixIdentity();
        IClickable* out = nullptr;
        float smallestZ = FLOAT_MAX;

        for(auto& clickable : m_clickableComponents)
        {
            if((mask & clickable->layers) == 0u)
            {
                continue;
            }

            //project clickable to screen space
            auto worldPos = clickable->parentTransform->GetPosition();
            auto worldPos_v = DirectX::XMLoadVector3(&worldPos);
            auto screenPos_v = DirectX::XMVector3Project(worldPos_v,
                                                         0.0f, 0.0f,
                                                         screenWidth, screenHeight,
                                                         0.0f, 1.0f,
                                                         projectionMatrix, viewMatrix, worldMatrix);
            auto screenPos = Vector3{};
            DirectX::XMStoreVector3(&screenPos, screenPos_v);

            //continue if there is a closer hit
            auto zDist = abs(unit.z - worldPos.z);
            if(zDist > smallestZ)
                continue;

            //scale bounding box by camera zoom amount
            const auto screenSpaceRadius = clickable->boundingBoxRadius / zDist;
            const auto top = screenPos.y - screenSpaceRadius;
            const auto bot = screenPos.y + screenSpaceRadius;
            const auto left = screenPos.x - screenSpaceRadius;
            const auto right = screenPos.x + screenSpaceRadius;

            if(const auto [x, y] = input::MousePos(); x > left && x < right && y > top && y < bot)
            {
                out = clickable;
                smallestZ = zDist;
            }
        }

        return out;
    }

    #ifdef NC_EDITOR_ENABLED
    void PhysicsSystem::UpdateWidgets(graphics::FrameManager* frameManager)
    {
        m_collisionSystem.UpdateWidgets(frameManager);
    }
    #endif
} //end namespace nc::physics