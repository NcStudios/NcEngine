#pragma once

#include "physics/IClickable.h"

#include <vector>

namespace nc::graphics
{
    #ifdef USE_VULKAN
    class Graphics2;
    #else
    class Graphics;
    #endif
}

namespace nc::physics
{
    class ClickableSystem
    {
        public:
            #ifdef USE_VULKAN
            ClickableSystem(graphics::Graphics2* graphics);
            #else
            ClickableSystem(graphics::Graphics* graphics);
            #endif

            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove) noexcept;
            IClickable* RaycastToClickables(LayerMask mask);
            void Clear();

        private:
            std::vector<IClickable*> m_clickableComponents;
            #ifdef USE_VULKAN
            graphics::Graphics2* m_graphics;
            #else
            graphics::Graphics* m_graphics;
            #endif
    };
}