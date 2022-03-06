#pragma once

#include "utility/SystemBase.h"

#include <memory>

namespace nc
{
    class NcEngine;

    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(NcEngine* engine) = 0;
            virtual void Unload() = 0;
    };

    class SceneSystem : public SystemBase
    {
        public:
            /* Queue a scene to be loaded upon completion of the current frame */
            virtual void ChangeScene(std::unique_ptr<Scene> scene) = 0;
    };
}