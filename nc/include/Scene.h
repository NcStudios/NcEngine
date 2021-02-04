#pragma once

#include <memory>

namespace nc::scene
{
    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load() = 0;
            virtual void Unload() = 0;
    };

    /* Queue a scene to be loaded upon completion of the current frame */
    void Change(std::unique_ptr<Scene>&& scene);
}