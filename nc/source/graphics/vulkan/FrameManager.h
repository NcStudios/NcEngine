#pragma once

#include "techniques/SimpleTechnique.h"
#include "techniques/TechniqueType.h"
#include "resources/VertexBuffer.h"
#include "resources/IndexBuffer.h"

#include "vulkan/vulkan.hpp"
#include <vector>
#include <memory>
#include <concepts>

namespace nc::graphics::vulkan
{
    class FrameManager
    {
        public:
            FrameManager();
            void RecordPasses();
            void RegisterModel();

            VertexBuffer GetVertexBuffer(TechniqueType techniqueType);
            IndexBuffer GetIndexBuffer(TechniqueType techniqueType);

        private:
            std::unique_ptr<SimpleTechnique> m_simpleTechnique;
            std::unordered_map<TechniqueType, std::vector<std::vector<Vertex>>> m_vertexBuffers;
            std::unordered_map<TechniqueType, std::vector<std::vector<uint32_t>>> m_indexBuffers;
    };

    class Model
    {
        public:
            Model();
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
    };


}