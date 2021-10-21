#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class VulkanScene : public Scene
    {
        public:
            void Load(NcEngine* engine) override;
            void Unload() override;
            void GenerateFloor(registry_type* registry, const std::vector<std::string>& meshPaths);
        private:
            SceneHelper m_sceneHelper;
    };
}