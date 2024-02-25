#pragma once

#include "ecs/Registry.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"

namespace nc::graphics
{
class PointLight;
class RenderGraph;

class Lighting
{
    public:
        Lighting(Registry* registry,
                 RenderGraph* renderGraph);
        void Clear();

    private:
        void OnCommitPointLightConnection();
        void OnRemovePointLightConnection();

        RenderGraph* m_renderGraph;
        // PPImageArrayBufferHandle m_postProcessImageArrayBuffer;
        Connection<PointLight&> m_onCommitPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
};
} // namespace nc::graphics
