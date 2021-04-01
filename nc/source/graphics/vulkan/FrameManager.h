#pragma once

#include "techniques/SimpleTechnique.h"
#include "techniques/TechniqueType.h"
#include "resources/VertexBuffer.h"
#include "resources/IndexBuffer.h"
#include "graphics/DummyModel.h"

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
            void RegisterModel(DummyModel* model);

            VertexBuffer* GetVertexBuffer(TechniqueType techniqueType);
            IndexBuffer* GetIndexBuffer(TechniqueType techniqueType);

        private:
            std::unique_ptr<SimpleTechnique> m_simpleTechnique;
            std::unordered_map<TechniqueType, std::vector<DummyModel*>> m_renderPairs;

            std::unordered_map<TechniqueType, vulkan::VertexBuffer> m_vertexBuffers;
            std::unordered_map<TechniqueType, vulkan::IndexBuffer> m_indexBuffers;
    };
}