#include "DebugWidgetSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Renderer.h"

namespace nc::ecs
{
    DebugWidgetSystem::DebugWidgetSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_graphics{graphics}
    {
        registry->RegisterOnAddCallback<vulkan::DebugWidget>
        (
            [this](vulkan::DebugWidget& debugWidget) { this->Add(debugWidget); }
        );

        registry->RegisterOnRemoveCallback<vulkan::DebugWidget>
        (
            [this](Entity entity) { this->Remove(entity); }
        );
    }

    void DebugWidgetSystem::Add(vulkan::DebugWidget& debugWidget)
    {
        m_graphics->GetRendererPtr()->RegisterDebugWidget(&debugWidget);
    }

    void DebugWidgetSystem::Remove(Entity entity)
    {
        m_graphics->GetRendererPtr()->DeregisterRenderable(entity);
    }

    void DebugWidgetSystem::Clear()
    {
        m_graphics->GetRendererPtr()->ClearDebugWidgets();
    }
}