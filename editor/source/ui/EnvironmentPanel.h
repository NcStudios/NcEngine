#pragma once

#include "framework/SceneData.h"

namespace nc
{
    class GraphicsModule;
    class Registry;
}

namespace nc::editor
{
    class AssetManifest;

    class EnvironmentPanel
    {
        public:
            EnvironmentPanel(SceneData* sceneData, Registry* registry, AssetManifest* assetManifest, GraphicsModule* graphics);

            void Draw();
            void DrawSkybox();

        private:
            SceneData* m_sceneData;
            Registry* m_registry;
            AssetManifest* m_assetManifest;
            GraphicsModule* m_graphics;
    };
}