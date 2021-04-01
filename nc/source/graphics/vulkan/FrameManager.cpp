#include "FrameManager.h"

namespace nc::graphics::vulkan
{
    FrameManager::FrameManager()
    :  m_simpleTechnique{ std::make_unique<SimpleTechnique>(this) },
       m_renderPairs{},
       m_vertexBuffers{},
       m_indexBuffers{}
    {
    }

    void FrameManager::RecordPasses()
    {
        m_simpleTechnique->Record();
    }

    void FrameManager::RegisterModel(DummyModel* dummyModel)
    {
        bool techniqueFound = false;

        for (auto& techniqueModelsPair : m_renderPairs)
        {
            if (techniqueModelsPair.first == dummyModel->techniqueType)
            {
                techniqueModelsPair.second.emplace_back(dummyModel);
                techniqueFound = true;
            }
        }

        if (!techniqueFound)
        {
            auto modelPtrs = std::vector<DummyModel*>{dummyModel};
            m_renderPairs.emplace(dummyModel->techniqueType, modelPtrs);
        }
    }

    VertexBuffer* FrameManager::GetVertexBuffer(TechniqueType techniqueType)
    {
        std::vector<Vertex> vertices = {};

        for (auto modelPtr : m_renderPairs.at(techniqueType))
        {
            vertices.insert(vertices.end(), modelPtr->vertices.begin(), modelPtr->vertices.end());
        }

        m_vertexBuffers.emplace(techniqueType, VertexBuffer(std::move(vertices)));

        return &m_vertexBuffers.at(techniqueType);
    }

    IndexBuffer* FrameManager::GetIndexBuffer(TechniqueType techniqueType)
    {
        std::vector<uint32_t> indices = {};

        for (auto modelPtr : m_renderPairs.at(techniqueType))
        {
            indices.insert(indices.end(), modelPtr->indices.begin(), modelPtr->indices.end());
        }

        m_indexBuffers.emplace(techniqueType, IndexBuffer(std::move(indices)));
        return &m_indexBuffers.at(techniqueType);
    }
}