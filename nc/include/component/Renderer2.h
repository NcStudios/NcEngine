#pragma once
#include "Component.h"

namespace nc
{
    class Renderer2 : public Component
    {
        public:
        Renderer2(EntityHandle handle)
        : Component{handle}
        {
        }

        private:
    };

    enum class TechniqueType : uint8_t
    {
        None = 0,
        Simple = 1
    };
}