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
    namespace vulkan { class MeshRenderer; }
}

namespace nc::graphics::vulkan
{
    class Commands;    

    class TechniqueManager
    {
        public:
            TechniqueManager(nc::graphics::Graphics2* graphics);
            void RegisterMeshRenderer(TechniqueType technique, MeshRenderer* meshRenderer);
            void RecordTechniques(Commands* commands);
            std::unique_ptr<TechniqueBase> CreateTechnique(TechniqueType techniqueType);
            void Clear();

        private:
            nc::graphics::Graphics2* m_graphics;
            std::vector<std::unique_ptr<TechniqueBase>> m_techniques;
    };
}