#include "ClickableSystem.h"
#include "Physics.h"
#include "Ecs.h"
#include "debug/Utils.h"
#include "Input.h"
#include "MainCamera.h"
#include "Window.h"
#include "directx/math/DirectXMath.h"
#include "graphics/Graphics2.h"

#include <algorithm>
#include <limits>

namespace
{
    nc::physics::ClickableSystem* impl = nullptr;
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

    /** System Implementation */
    ClickableSystem::ClickableSystem
    (
        graphics::Graphics2* graphics
    )
        : m_clickableComponents{},
          m_graphics{graphics}
    {
        impl = this;
    }

    void ClickableSystem::RegisterClickable(IClickable* toAdd)
    {
        m_clickableComponents.push_back(toAdd);
    }

    void ClickableSystem::UnregisterClickable(IClickable* toRemove) noexcept
    {
        auto pos = std::find(m_clickableComponents.begin(), m_clickableComponents.end(), toRemove);
        if(pos != m_clickableComponents.end())
        {
            *pos = m_clickableComponents.back();
            m_clickableComponents.pop_back();
        }
    }

    IClickable* ClickableSystem::RaycastToClickables(LayerMask mask)
    {
        auto unit = Normalize(Vector3::Splat(1.0f));
        auto unit_v = DirectX::XMLoadVector3(&unit);
        const auto& viewMatrix = m_graphics->GetViewMatrix();
        unit_v = DirectX::XMVector3Transform(unit_v, viewMatrix);
        DirectX::XMStoreVector3(&unit, unit_v);

        auto [screenWidth, screenHeight] = window::GetDimensions();
        const auto& projectionMatrix = m_graphics->GetProjectionMatrix();
        auto worldMatrix = DirectX::XMMatrixIdentity();
        IClickable* out = nullptr;
        float smallestZ = std::numeric_limits<float>::max();
        auto* registry = ActiveRegistry();

        for(auto& clickable : m_clickableComponents)
        {
            if((mask & clickable->layers) == 0u)
                continue;

            //project clickable to screen space
            auto worldPos = registry->Get<Transform>(clickable->entity)->GetPosition();
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

    void ClickableSystem::Clear()
    {
        m_clickableComponents.clear();
    }
}