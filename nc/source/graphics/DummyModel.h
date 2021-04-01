#pragma once

#include "vulkan/techniques/TechniqueType.h"
#include "graphics/vulkan/resources/VertexBuffer.h" // Temporary, just to get Vertex for DummyModel, which will be created elsewhere

#include <vector>

namespace nc::graphics
{
    class DummyModel
    {
        public:
            DummyModel();
            vulkan::TechniqueType techniqueType;
            std::vector<vulkan::Vertex> vertices;
            std::vector<uint32_t> indices;
    };
}