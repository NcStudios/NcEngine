#pragma once

#include "LayerMask.h"

/** Call NcRegisterAsClickable and NcUnregisterAsClickable
 *  to enable/disable testing against an IClickable. An
 *  IClickable _MUST_ not be registered after it is destroyed.
 */

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
    };
}