#pragma once

#include "graphics/ResourceGroup.h"

namespace nc::graphics
{
    class Model;

    class Step : public ResourceGroup
    {
        public:
            Step(size_t targetPass);

            size_t targetPass; /** @todo: Replace with enum or do templating **/
    };
}