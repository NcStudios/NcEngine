#pragma once

#include "framework/SceneData.h"

namespace nc
{
    class Environment;
}

namespace nc::editor
{
    class AssetManifest;

    class Environment2
    {
        public:
            Environment2(SceneData* sceneData, AssetManifest* assetManifest, Environment* environment);

            void Draw();
            void DrawSkybox();

        private:
            SceneData* m_sceneData;
            AssetManifest* m_assetManifest;
            Environment* m_environment;
    };
}