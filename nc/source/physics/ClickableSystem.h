#pragma once

#include "physics/IClickable.h"

#include <vector>

namespace nc::physics
{
    class ClickableSystem
    {
        public:
            ClickableSystem();

            void RegisterClickable(IClickable* toAdd);
            void UnregisterClickable(IClickable* toRemove) noexcept;
            IClickable* RaycastToClickables(LayerMask mask);
            void Clear();

        private:
            std::vector<IClickable*> m_clickableComponents;
    };
}