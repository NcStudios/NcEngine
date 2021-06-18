#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class VulkanScene : public scene::Scene
    {
        public:
            void Load(registry_type* registry) override;
            void Unload() override;
            void GenerateFloor(const std::vector<std::string>& meshPaths);
        private:
            SceneHelper m_sceneHelper;
    };
}