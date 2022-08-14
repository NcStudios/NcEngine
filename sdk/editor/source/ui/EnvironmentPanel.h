#pragma once

#include "framework/SceneData.h"

namespace nc
{
class Registry;
namespace graphics { struct NcGraphics; }
}

namespace nc::editor
{
    class AssetManifest;

    class EnvironmentPanel
    {
        public:
            EnvironmentPanel(SceneData* sceneData, Registry* registry, AssetManifest* assetManifest, graphics::NcGraphics* graphics);

            void Draw();
            void DrawSkybox();

        private:
            SceneData* m_sceneData;
            Registry* m_registry;
            AssetManifest* m_assetManifest;
            graphics::NcGraphics* m_graphics;
    };
}