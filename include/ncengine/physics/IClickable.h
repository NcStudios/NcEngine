#pragma once

#include "LayerMask.h"

namespace nc::physics
{
class IClickable
{
    public:
        IClickable(Entity entity_, float radius)
            : entity{entity_},
              boundingBoxRadius{radius},
              layers{LayerMaskNone}
        {
        }

        virtual void OnClick() = 0;

        Entity entity;
        float boundingBoxRadius;
        LayerMask layers;
    
    protected:
        ~IClickable() = default;
};
} // namespace nc::physics
