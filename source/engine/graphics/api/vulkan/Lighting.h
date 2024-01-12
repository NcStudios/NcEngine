#pragma once

#include "ecs/Registry.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class PointLight;
class RenderGraph;
struct ShaderResources;

class Lighting
{
    public:
        Lighting(Registry* registry,
                 RenderGraph* renderGraph,
                 ShaderResources* shaderResources);

        void Clear();
        void Resize();

    private:
        void OnAddPointLightConnection();
        void OnRemovePointLightConnection();

        RenderGraph* m_renderGraph;
        ShaderResources* m_shaderResources;
        Connection<PointLight&> m_onAddPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
};
}