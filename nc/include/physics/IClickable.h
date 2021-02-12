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
                  layers{LayerMaskNone}
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