#pragma once

#include "utility/SystemBase.h"

#include <memory>

namespace nc
{
    class nc_engine;

    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(nc_engine* engine) = 0;
            virtual void Unload() = 0;
    };

    class SceneSystem : public SystemBase
    {
        public:
            /* Queue a scene to be loaded upon completion of the current frame */
            virtual void ChangeScene(std::unique_ptr<Scene> scene) = 0;
    };
}