#include "PhysicsSystem.h"
#include "Physics.h"
#include "DebugUtils.h"
#include "Input.h"
#include "component/Transform.h"
#include "Window.h"
#include "graphics/Graphics.h"
#include "MainCamera.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace
{
    const auto FLOAT_MAX = std::numeric_limits<float>::max();
    nc::physics::PhysicsSystem* impl = nullptr;
}

namespace nc::physics
{
    /* Api Implementation */
    void RegisterClickable(IClickable* clickable)
    {
        IF_THROW(!impl, "physics::RegisterClickable - impl is not set");
        impl->RegisterClickable(clickable);
    }

    void UnregisterClickable(IClickable* clickable)
    {
        IF_THROW(!impl, "physics::UnregisterClickable - impl is not set");
        impl->UnregisterClickable(clickable);
    }

    IClickable* RaycastToClickables(LayerMask mask)
    {
        IF_THROW(!impl, "physics::RaycastToClickables - impl is not set");
        return impl->RaycastToClickables(mask);
    }

    /* Physics System */
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics)
        : m_clickableComponents{},
          m_graphics{ graphics }
    {
        impl = this;
    }

    void PhysicsSystem::RegisterClickable(IClickable* toAdd)
    {
        m_clickableComponents.push_back(toAdd);
    }

    void PhysicsSystem::UnregisterClickable(IClickable* toRemove)
    {
        auto beg = std::begin(m_clickableComponents);
        auto end = std::end(m_clickableComponents);
        auto pos = std::find(beg, end, toRemove);
        if(pos == end)
            throw std::runtime_error("Attempt to unregister an unregistered IClickable");

        *pos = m_clickableComponents.back();
        m_clickableComponents.pop_back();
    }

    IClickable* PhysicsSystem::RaycastToClickables(LayerMask mask)
    {
        auto unit = Normalize(Vector3::Splat(1.0f));
        auto unit_v = DirectX::XMLoadVector3(&unit);
        auto viewMatrix = camera::GetMainCameraTransform()->GetViewMatrix();
        unit_v = DirectX::XMVector3Transform(unit_v, viewMatrix);
        DirectX::XMStoreVector3(&unit, unit_v);

        auto [screenWidth, screenHeight] = window::GetDimensions();
        auto projectionMatrix = m_graphics->GetProjectionMatrix();
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
} //end namespace nc::physics