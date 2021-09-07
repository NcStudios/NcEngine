#pragma once

#include "physics/IClickable.h"

#include <vector>

namespace nc::graphics
{
    class Graphics;
}

namespace nc::physics
{
    class ClickableSystem
    {
        public:
            ClickableSystem(graphics::Graphics* graphics);

            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove) noexcept;
            IClickable* RaycastToClickables(LayerMask mask);
            void Clear();

        private:
            std::vector<IClickable*> m_clickableComponents;
            graphics::Graphics* m_graphics;
    };
}