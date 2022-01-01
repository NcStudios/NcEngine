#pragma once

#include "framework/SceneData.h"

namespace nc
{
    class Environment;
    class Registry;
}

namespace nc::editor
{
    class AssetManifest;

    class EnvironmentPanel
    {
        public:
            EnvironmentPanel(SceneData* sceneData, Registry* registry, AssetManifest* assetManifest, Environment* environment);

            void Draw();
            void DrawSkybox();

        private:
            SceneData* m_sceneData;
            Registry* m_registry;
            AssetManifest* m_assetManifest;
            Environment* m_environment;
    };
}