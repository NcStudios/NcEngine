#pragma once

#include "graphics/ResourceGroup.h"

namespace nc::graphics
{
    class Model;

    class Step : public ResourceGroup
    {
        public:
            explicit Step(size_t pass)
                : targetPass{pass}
            {}

            size_t targetPass; /** @todo: Replace with enum or do templating **/
    };
}