#pragma once

#include "Ecs.h"

#include <memory>

namespace nc::scene
{
    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(registry_type* registry) = 0;
            virtual void Unload() = 0;
    };

    /* Queue a scene to be loaded upon completion of the current frame */
    void Change(std::unique_ptr<Scene> scene);
}