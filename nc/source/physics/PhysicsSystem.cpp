#include "PhysicsSystem.h"
#include "input/Input.h"
#include "component/Transform.h"
#include "Window.h"
#include "graphics/Graphics.h"
#include "camera/MainCamera.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace nc::physics
{
    PhysicsSystem* PhysicsSystem::m_instance = nullptr;

    PhysicsSystem::PhysicsSystem(graphics::Graphics* graphics)
        : m_clickableComponents{},
          m_graphics{ graphics }
    {
        PhysicsSystem::m_instance = this;
    }

    PhysicsSystem::~PhysicsSystem()
    {
        PhysicsSystem::m_instance = nullptr;
    }

    void PhysicsSystem::RegisterClickable(IClickable* toAdd)
    {
        PhysicsSystem::m_instance->RegisterClickable_(toAdd);
    }

    void PhysicsSystem::UnregisterClickable(IClickable* toRemove)
    {
        PhysicsSystem::m_instance->UnregisterClickable_(toRemove);
    }

    IClickable* PhysicsSystem::RaycastToClickables(LayerMask mask)
    {
        return PhysicsSystem::m_instance->RaycastToClickables_(mask);
    }

    void PhysicsSystem::RegisterClickable_(IClickable* toAdd)
    {
        m_clickableComponents.push_back(toAdd);
    }

    void PhysicsSystem::UnregisterClickable_(IClickable* toRemove)
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

    IClickable* PhysicsSystem::RaycastToClickables_(LayerMask mask)
    {
        auto windowDimensions = Window::GetDimensions();
        auto viewMatrix = camera::MainCamera::GetTransform()->CamGetMatrix();
        auto projectionMatrix = m_graphics->GetProjection();
        auto worldMatrix = DirectX::XMMatrixIdentity();
        auto unit = Vector3(1,1,1).GetNormalized().GetXMFloat3();
        auto unit_v = DirectX::XMLoadFloat3(&unit);
        unit_v = DirectX::XMVector3Transform(unit_v, viewMatrix);
        DirectX::XMStoreFloat3(&unit, unit_v);

        IClickable* out = nullptr;
        float smallestZ = std::numeric_limits<float>::max();

        for(auto& clickable : m_clickableComponents)
        {
            if((mask & clickable->layers) == 0u)
            {
                continue;
            }

            //project clickable to screen space
            DirectX::XMFLOAT3 worldPos = clickable->parentTransform->GetPosition().GetXMFloat3();
            auto screenPos = DirectX::XMFLOAT3{};
            auto worldPos_v = DirectX::XMLoadFloat3(&worldPos);
            auto screenPos_v = DirectX::XMVector3Project(worldPos_v,
                                                         0.0f, 0.0f,
                                                         windowDimensions.X(), windowDimensions.Y(),
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