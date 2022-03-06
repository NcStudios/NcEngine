#pragma once

#include "Scene.h"

#include <memory>

namespace nc::scene
{
    class SceneSystemImpl final : public SceneSystem
    {
        public:
            SceneSystemImpl();

            void ChangeScene(std::unique_ptr<Scene> swapScene) override;
            bool IsSceneChangeScheduled() const;
            void UnloadActiveScene();
            void DoSceneChange(nc_engine* engine);

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            bool m_isSceneChangeScheduled;
    };
}