#pragma once

#include "ecs/Registry.h"
#include "graphics/Renderer.h"

namespace nc::graphics
{
    class PointLight;
    class Renderer;

    class Shadows
    {
        public:
            Shadows(Renderer* renderer);

        private:
            Renderer* m_renderer;
            Connection<PointLight&> m_onAddPointLightConnection;
            Connection<Entity> m_onRemovePointLightConnection;
            uint32_t m_numShadowCasters;
    };
}