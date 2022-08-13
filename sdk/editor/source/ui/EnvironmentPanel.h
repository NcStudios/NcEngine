#pragma once

#include "framework/SceneData.h"

namespace nc
{
class Registry;
namespace graphics { struct GraphicsModule; }
}

namespace nc::editor
{
    class AssetManifest;

    class EnvironmentPanel
    {
        public:
            EnvironmentPanel(SceneData* sceneData, Registry* registry, AssetManifest* assetManifest, graphics::GraphicsModule* graphics);

            void Draw();
            void DrawSkybox();

        private:
            SceneData* m_sceneData;
            Registry* m_registry;
            AssetManifest* m_assetManifest;
            graphics::GraphicsModule* m_graphics;
    };
}