#pragma once
#include <memory>
#include "ComponentSystem.h"
#include "PointLight.h"


namespace nc::graphics { class Graphics; }

namespace nc::engine
{
    class LightSystem : public ComponentSystem<PointLight>
    {
        public:
            LightSystem();
            ~LightSystem();
            LightSystem(const LightSystem&) = delete;
            LightSystem(LightSystem&&) = delete;

            void BindLights(graphics::Graphics * graphics);
    };
} //end namespace nc::engine