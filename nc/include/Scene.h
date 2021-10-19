#pragma once

#include "Ecs.h"

#include <memory>

namespace nc
{
    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(registry_type* registry) = 0;
            virtual void Unload() = 0;
    };

    /* Queue a scene to be loaded upon completion of the current frame */
    void ChangeScene(std::unique_ptr<Scene> scene);
}