#pragma once

#include "graphics/vulkan/TechniqueType.h"
#include "graphics/vulkan/Mesh.h"
#include "techniques/TechniqueBase.h"

#include <vector>
#include <memory>

namespace nc { class Transform; }

namespace nc::graphics::vulkan
{
    class Commands;    

    struct GlobalData
    {
        vk::Buffer* vertexBuffer;
        vk::Buffer* indexBuffer;
    };

    class TechniqueManager
    {
        public:
            TechniqueManager();
            void RegisterRenderer(TechniqueType technique, Mesh mesh, Transform* transform);
            void RecordTechniques(Commands* commands);
            void RegisterGlobalData(vk::Buffer* vertexBuffer, vk::Buffer* indexBuffer);
            std::unique_ptr<TechniqueBase> CreateTechnique(TechniqueType techniqueType);
            void Clear();

        private:
            std::vector<std::unique_ptr<TechniqueBase>> m_techniques;
            GlobalData m_globalData;
    };
}