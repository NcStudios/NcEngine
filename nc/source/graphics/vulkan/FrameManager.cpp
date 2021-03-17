#include "FrameManager.h"

namespace nc::graphics::vulkan
{
    FrameManager::FrameManager()
    :  m_simpleTechnique{ std::make_unique<SimpleTechnique>(this) },
       m_vertexBuffers{},
       m_indexBuffers{}
    {
    }

    void FrameManager::RecordPasses()
    {
        m_simpleTechnique->Record();
    }

    void FrameManager::RegisterModel()
    {
        auto dummyModel = Model{};

        bool techniqueFoundVb = false;
        for (auto& techniqueBufferPair : m_vertexBuffers)
        {
            if (techniqueBufferPair.first == TechniqueType::Simple)
            {
                techniqueBufferPair.second.emplace_back(dummyModel.vertices);
                techniqueFoundVb = true;
                break;
            }
        }

        if (!techniqueFoundVb)
        {
            auto vertexVector = std::vector<std::vector<Vertex>>{dummyModel.vertices};
            m_vertexBuffers.emplace(std::make_pair(TechniqueType::Simple, std::move(vertexVector)));
        }


        bool techniqueFoundIb = false;
        for (auto& techniqueBufferPair : m_indexBuffers)
        {
            if (techniqueBufferPair.first == TechniqueType::Simple)
            {
                techniqueBufferPair.second.emplace_back(dummyModel.indices);
                techniqueFoundIb = true;
                break;
            }
        }

        if (!techniqueFoundIb)
        {
            auto indexVector = std::vector<std::vector<uint32_t>>{dummyModel.indices};

            m_indexBuffers.emplace(std::make_pair(TechniqueType::Simple, std::move(indexVector)));
        }
    }

    VertexBuffer FrameManager::GetVertexBuffer(TechniqueType techniqueType)
    {
        std::vector<Vertex> vertices = {};
        auto vertexBuffers = m_vertexBuffers.at(techniqueType);
        for (auto& vertexBuffer : vertexBuffers)
        {
            for (auto& vertex : vertexBuffer)
            {
                vertices.emplace_back(vertex);
            }
        }
        return VertexBuffer(vertices);
    }

    IndexBuffer FrameManager::GetIndexBuffer(TechniqueType techniqueType)
    {
        std::vector<uint32_t> indices = {};
        auto indexBuffers = m_indexBuffers.at(techniqueType);
        for (auto& indexBuffer : indexBuffers)
        {
            for (auto& index : indexBuffer)
            {
                indices.emplace_back(index);
            }
        }
        
        return IndexBuffer(indices);
    }

    Model::Model()
    {
        Vertex vertex;
        vertex.Position  = Vector3{0.5, 0.5, -0.5};
        vertex.Normal    = Vector3{0, 0, -1};
        vertex.UV        = Vector2{0.375, 1};
        vertex.Tangent   = Vector3{-1, 0, 0};
        vertex.Bitangent = Vector3{0, -1, 0};

        Vertex vertex2;
        vertex2.Position  = Vector3{-0.5, 0.5, -0.5};
        vertex2.Normal    = Vector3{-0, 0, -1};
        vertex2.UV        = Vector2{0.625, 1};
        vertex2.Tangent   = Vector3{-1, 0, 0};
        vertex2.Bitangent = Vector3{0, -1, 0};

        Vertex vertex3;
        vertex3.Position  = Vector3{-0.5, -0.5, -0.5};
        vertex3.Normal    = Vector3{0, 0, -1};
        vertex3.UV        = Vector2{0, 0};
        vertex3.Tangent   = Vector3{-1, 0, 0};
        vertex3.Bitangent = Vector3{0, -1, 0};

        Vertex vertex4;
        vertex4.Position  = Vector3{0.5, -0.5, -0.5};
        vertex4.Normal    = Vector3{0, 0, -1};
        vertex4.UV        = Vector2{0.375, 0.75};
        vertex4.Tangent   = Vector3{-1, 0, 0};
        vertex4.Bitangent = Vector3{0, -1, 0};

        Vertex vertex5;
        vertex5.Position  = Vector3{0.5, -0.5, 0.5};
        vertex5.Normal    = Vector3{0, -1, 0};
        vertex5.UV        = Vector2{0.375, 0.75};
        vertex5.Tangent   = Vector3{0, 0, -1};
        vertex5.Bitangent = Vector3{-1, 0, 0};

        Vertex vertex6;
        vertex6.Position  = Vector3{0.5, -0.5, -0.5};
        vertex6.Normal    = Vector3{0, -1, 0};
        vertex6.UV        = Vector2{0.625, 0.75};
        vertex6.Tangent   = Vector3{0, 0, -1};
        vertex6.Bitangent = Vector3{-1, 0, 0};

        Vertex vertex7;
        vertex7.Position  = Vector3{-0.5, -0.5, -0.5};
        vertex7.Normal    = Vector3{0, -1, 0};
        vertex7.UV        = Vector2{0.625, 0.5};
        vertex7.Tangent   = Vector3{0, 0, -1};
        vertex7.Bitangent = Vector3{-1, 0, 0};

        Vertex vertex8;
        vertex8.Position  = Vector3{-0.5, -0.5, 0.5};
        vertex8.Normal    = Vector3{0, -1, 0};
        vertex8.UV        = Vector2{0.375, 0.5};
        vertex8.Tangent   = Vector3{0, 0, -1};
        vertex8.Bitangent = Vector3{-1, 0, 0};

        Vertex vertex9;
        vertex9.Position  = Vector3{-0.5, -0.5, 0.5};
        vertex9.Normal    = Vector3{-1, 0, 0};
        vertex9.UV        = Vector2{0.375, 0.5};
        vertex9.Tangent   = Vector3{0, 0, -1};
        vertex9.Bitangent = Vector3{0, 1, 0};

        Vertex vertex10;
        vertex10.Position  = Vector3{-0.5, -0.5, -0.5};
        vertex10.Normal    = Vector3{-1, 0, 0};
        vertex10.UV        = Vector2{0.625, 0.5};
        vertex10.Tangent   = Vector3{0, 0, -1};
        vertex10.Bitangent = Vector3{0, 1, 0};

        Vertex vertex11;
        vertex11.Position  = Vector3{-0.5, 0.5, -0.5};
        vertex11.Normal    = Vector3{-1, 0, 0};
        vertex11.UV        = Vector2{0.625, 0.25};
        vertex11.Tangent   = Vector3{0, 0, -1};
        vertex11.Bitangent = Vector3{0, 1, 0};

        Vertex vertex12;
        vertex12.Position  = Vector3{-0.5, 0.5, 0.5};
        vertex12.Normal    = Vector3{-1, 0, 0};
        vertex12.UV        = Vector2{0.375, 0.25};
        vertex12.Tangent   = Vector3{0, 0, -1};
        vertex12.Bitangent = Vector3{0, 1, 0};

        Vertex vertex13;
        vertex13.Position  = Vector3{-0.5, 0.5, 0.5};
        vertex13.Normal    = Vector3{0, 0, 1};
        vertex13.UV        = Vector2{0.375, 0.25};
        vertex13.Tangent   = Vector3{1, 0, 0};
        vertex13.Bitangent = Vector3{0, -1, 0};

        Vertex vertex14;
        vertex14.Position  = Vector3{0.5, 0.5, 0.5};
        vertex14.Normal    = Vector3{0, 0, 1};
        vertex14.UV        = Vector2{0.625, 0.25};
        vertex14.Tangent   = Vector3{1, 0, 0};
        vertex14.Bitangent = Vector3{0, -1, 0};

        Vertex vertex15;
        vertex15.Position  = Vector3{0.5, -0.5, 0.5};
        vertex15.Normal    = Vector3{0, 0, 1};
        vertex15.UV        = Vector2{0.625, 0};
        vertex15.Tangent   = Vector3{1, 0, 0};
        vertex15.Bitangent = Vector3{0, -1, 0};

        Vertex vertex16;
        vertex16.Position  = Vector3{-0.5, -0.5, 0.5};
        vertex16.Normal    = Vector3{0, 0, 1};
        vertex16.UV        = Vector2{0.375, 0};
        vertex16.Tangent   = Vector3{1, 0, 0};
        vertex16.Bitangent = Vector3{0, -1, 0};

        Vertex vertex17;
        vertex17.Position  = Vector3{0.5, 0.5, 0.5};
        vertex17.Normal    = Vector3{1, 0, 0};
        vertex17.UV        = Vector2{0.125, 0.5};
        vertex17.Tangent   = Vector3{0, 0, -1};
        vertex17.Bitangent = Vector3{0, -1, 0};

        Vertex vertex18;
        vertex18.Position  = Vector3{0.5, 0.5, -0.5};
        vertex18.Normal    = Vector3{1, 0, 0};
        vertex18.UV        = Vector2{0.375, 0.5};
        vertex18.Tangent   = Vector3{0, 0, -1};
        vertex18.Bitangent = Vector3{0, -1, 0};

        Vertex vertex19;
        vertex19.Position  = Vector3{0.5, -0.5, -0.5};
        vertex19.Normal    = Vector3{1, 0, 0};
        vertex19.UV        = Vector2{0.375, 0.25};
        vertex19.Tangent   = Vector3{0, 0, -1};
        vertex19.Bitangent = Vector3{0, -1, 0};

        Vertex vertex20;
        vertex20.Position  = Vector3{0.5, -0.5, 0.5};
        vertex20.Normal    = Vector3{1, 0, 0};
        vertex20.UV        = Vector2{0.125, 0.25};
        vertex20.Tangent   = Vector3{0, 0, -1};
        vertex20.Bitangent = Vector3{0, -1, 0};

        Vertex vertex21;
        vertex21.Position  = Vector3{-0.5, 0.5, 0.5};
        vertex21.Normal    = Vector3{0, 1, 0};
        vertex21.UV        = Vector2{0.625, 0.5};
        vertex21.Tangent   = Vector3{0, 0, -1};
        vertex21.Bitangent = Vector3{1, 0, 0};

        Vertex vertex22;
        vertex22.Position  = Vector3{-0.5, 0.5, -0.5};
        vertex22.Normal    = Vector3{0, 1, 0};
        vertex22.UV        = Vector2{0.875, 0.5};
        vertex22.Tangent   = Vector3{0, 0, -1};
        vertex22.Bitangent = Vector3{1, 0, 0};

        Vertex vertex23;
        vertex23.Position  = Vector3{0.5, 0.5, -0.5};
        vertex23.Normal    = Vector3{0, 1, 0};
        vertex23.UV        = Vector2{0.875, 0.25};
        vertex23.Tangent   = Vector3{0, 0, -1};
        vertex23.Bitangent = Vector3{1, 0, 0};

        Vertex vertex24;
        vertex24.Position  = Vector3{0.5, 0.5, 0.5};
        vertex24.Normal    = Vector3{0, 1, 0};
        vertex24.UV        = Vector2{0.625, 0.25};
        vertex24.Tangent   = Vector3{0, 0, -1};
        vertex24.Bitangent = Vector3{1, 0, 0};

        vertices = {vertex, vertex2, vertex3, vertex4, vertex5, vertex6, vertex7, vertex8, vertex9, vertex10,
                    vertex11, vertex12, vertex13, vertex14, vertex15, vertex16, vertex17, vertex18, vertex19, vertex20,
                    vertex21, vertex22, vertex23, vertex24};
        
        indices = {3, 2, 1, 3, 1, 0, 7, 6, 5, 7, 5, 4, 11, 10, 9, 11, 9, 8, 15, 14, 13, 15, 13, 12, 19, 18, 17, 19, 17, 16, 23, 22, 21, 23, 21, 20};
    }

}