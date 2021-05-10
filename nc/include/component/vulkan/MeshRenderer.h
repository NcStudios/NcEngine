#pragma once

#include "component/Component.h"

namespace nc { class Transform; }

namespace nc::vulkan
{
    class MeshRenderer : public Component
    {
        public:
            MeshRenderer(EntityHandle handle);
            Transform* GetTransform();
    };
}