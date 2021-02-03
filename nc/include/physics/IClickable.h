#pragma once

#include "LayerMask.h"

namespace nc { class Transform; }

namespace nc::physics
{
    class IClickable
    {
        public:
            IClickable(Transform* transform, float radius)
                : parentTransform{transform},
                  boundingBoxRadius{radius},
                  layers{LAYER_MASK_NONE}
            {
            }

            virtual void OnClick() = 0;

            Transform* parentTransform;
            float boundingBoxRadius;
            LayerMask layers;
        
        protected:
            ~IClickable() = default;
    };
}