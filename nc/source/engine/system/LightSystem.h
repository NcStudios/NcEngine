#pragma once
#include <memory>
#include "ComponentSystem.h"
#include "component/PointLight.h"


namespace nc::graphics { class Graphics; }

namespace nc::engine::system
{
    class LightSystem : public ComponentSystem<PointLight>
    {
        public:
            LightSystem();
            ~LightSystem();
            LightSystem(const LightSystem&) = delete;
            LightSystem(LightSystem&&) = delete;

            void BindLights();
    };
} //end namespace nc::engine