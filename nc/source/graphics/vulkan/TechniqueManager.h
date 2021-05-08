#pragma once

#include "graphics/vulkan/TechniqueType.h"
#include "graphics/vulkan/Mesh.h"
#include "techniques/TechniqueBase.h"

#include <vector>
#include <memory>

namespace nc 
{ 
    class Transform;
    namespace graphics { class Graphics2; }
}

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
            TechniqueManager(nc::graphics::Graphics2* graphics);
            void RegisterRenderer(TechniqueType technique, Mesh mesh, Transform* transform);
            void RecordTechniques(Commands* commands);
            void RegisterGlobalData(vk::Buffer* vertexBuffer, vk::Buffer* indexBuffer);
            std::unique_ptr<TechniqueBase> CreateTechnique(TechniqueType techniqueType);
            void Clear();

        private:
            nc::graphics::Graphics2* m_graphics;
            std::vector<std::unique_ptr<TechniqueBase>> m_techniques;
            GlobalData m_globalData;
    };
}