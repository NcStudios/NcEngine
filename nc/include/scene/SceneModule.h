#pragma once

#include "Module.h"
#include "Scene.h"

namespace nc
{
    class SceneModule : public Module
    {
        public:
            /** @brief Queue a scene to be loaded upon completion of the current frame */
            virtual void ChangeScene(std::unique_ptr<Scene> scene) = 0;

            /** @brief Unload the current scene and load the queued scene.
             *  
             * The default runtime will call this automatically once all tasks have finished
             * if a scene has been queued with ChangeScene.
             */
            virtual void DoSceneSwap(NcEngine* engine) = 0;
    };
}