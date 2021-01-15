#include "PhysicsSystem.h"
#include "input/Input.h"
#include "component/Transform.h"
#include "Window.h"
#include "graphics/Graphics.h"
#include "MainCamera.h"

#include "directx/math/DirectXMath.h"
#include "directx/math/DirectXCollision.h"
#include "component/Collider.h"
#include <iostream>

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace
{
    const auto FLOAT_MAX = std::numeric_limits<float>::max();
}

namespace nc::physics
{
    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics)
        : m_clickableComponents{},
          m_graphics{ graphics }
    {
    }

    void PhysicsSystem::CheckCollisions(std::vector<Collider*> colliders)
    {
        DirectX::BoundingOrientedBox unit, a, b;
        const auto count = colliders.size();

        for(size_t i = 0u; i < count; ++i)
        {
            unit.Transform(a, colliders[i]->GetTransformationMatrix());

            for(size_t j = i + 1; j < count; ++j)
            {
                unit.Transform(b, colliders[j]->GetTransformationMatrix());
                std::cout << "Result: " << i << ", " << j << " | ";
                if(a.Intersects(b))
                    std::cout << "Intersection\n";
                else
                    std::cout << "No Intersection\n";
            }
        }
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
        {
            throw std::runtime_error("Attempt to unregister an unregistered IClickable");
        }

        *pos = m_clickableComponents.back();
        m_clickableComponents.pop_back();
    }

    IClickable* PhysicsSystem::RaycastToClickables(LayerMask mask)
    {
        auto [screenWidth, screenHeight] = Window::GetDimensions();
        auto viewMatrix = camera::MainCamera::GetTransform()->GetViewMatrix();
        auto projectionMatrix = m_graphics->GetProjectionMatrix();
        auto worldMatrix = DirectX::XMMatrixIdentity();
        auto unit = Vector3(1,1,1).GetNormalized().ToXMFloat3();
        auto unit_v = DirectX::XMLoadFloat3(&unit);
        unit_v = DirectX::XMVector3Transform(unit_v, viewMatrix);
        DirectX::XMStoreFloat3(&unit, unit_v);

        IClickable* out = nullptr;
        float smallestZ = FLOAT_MAX;

        for(auto& clickable : m_clickableComponents)
        {
            if((mask & clickable->layers) == 0u)
            {
                continue;
            }

            //project clickable to screen space
            DirectX::XMFLOAT3 worldPos = clickable->parentTransform->GetPosition().ToXMFloat3();
            auto screenPos = DirectX::XMFLOAT3{};
            auto worldPos_v = DirectX::XMLoadFloat3(&worldPos);
            auto screenPos_v = DirectX::XMVector3Project(worldPos_v,
                                                         0.0f, 0.0f,
                                                         screenWidth, screenHeight,
                                                         0.0f, 1.0f,
                                                         projectionMatrix, viewMatrix, worldMatrix);
            DirectX::XMStoreFloat3(&screenPos, screenPos_v);

            //continue if there is a closer hit
            auto zDist = abs(unit.z - worldPos.z);
            if(zDist > smallestZ)
            {
                continue;
            }

            //scale bounding box by camera zoom amount
            auto screenSpaceRadius = clickable->boundingBoxRadius / zDist;
            auto top = screenPos.y - screenSpaceRadius;
            auto bot = screenPos.y + screenSpaceRadius;
            auto left = screenPos.x - screenSpaceRadius;
            auto right = screenPos.x + screenSpaceRadius;

            if(input::MouseX > left &&
               input::MouseX < right &&
               input::MouseY > top &&
               input::MouseY < bot)
            {
                out = clickable;
                smallestZ = zDist;
            }
        }

        return out;
    }
} //end namespace nc::physics