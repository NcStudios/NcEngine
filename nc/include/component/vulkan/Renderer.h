#pragma once

#include "component/Component.h"

namespace nc { class Transform; }

namespace nc::vulkan
{
    class Renderer : public Component
    {
        public:
            Renderer(EntityHandle handle);
            Transform* GetTransform();

        private: 
            Transform* m_transform;
    };
}