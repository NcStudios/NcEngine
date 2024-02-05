#pragma once

#include "ecs/Registry.h"

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
        void OnCommitPointLightConnection();
        void OnRemovePointLightConnection();

        RenderGraph* m_renderGraph;
        ShaderResources* m_shaderResources;
        Connection<PointLight&> m_onCommitPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
};
} // namespace nc::graphics
