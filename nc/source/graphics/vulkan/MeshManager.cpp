#include "graphics/vulkan/MeshManager.h"
#include "graphics/Graphics2.h"
#include "debug/Utils.h"

#include <fstream>

namespace
{
    using namespace nc::graphics::vulkan;

    MeshManager* impl = nullptr;

    bool HasValidAssetExtension(const std::string& path) 
    {
        std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension.compare("nca") == 0 ? true : false;
    }

    void ReadVerticesFromAsset(std::ifstream* file, std::vector<Vertex>* out, size_t count)
    {
        nc::Vector3 ver, nrm, tan, bit;
        nc::Vector2 tex;

        for(size_t i = 0; i < count; ++i)
        {
            if(file->fail())
                throw std::runtime_error("ReadVerticesFromAsset - Failure");
            
            *file >> ver.x >> ver.y >> ver.z
                 >> nrm.x >> nrm.y >> nrm.z
                 >> tex.x >> tex.y
                 >> tan.x >> tan.y >> tan.z
                 >> bit.x >> bit.y >> bit.z;

            out->emplace_back(ver, nrm, tex, tan, bit);
        }
    }

    void ReadIndicesFromAsset(std::ifstream* file, std::vector<uint32_t>* out, size_t count)
    {
        uint32_t index; //@todo: Update vulkan descriptors to accept 16_t and change here to 16_t
        for(size_t i = 0; i < count; ++i)
        {
            if(file->fail())
                throw std::runtime_error("ReadIndicesFromAsset - Failure");

            *file >> index;
            out->push_back(index);
        }
    }

} // end anonymous namespace

namespace nc::graphics::vulkan
{
    vk::VertexInputBindingDescription GetVertexBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0);
        bindingDescription.setStride(sizeof(Vertex));
        bindingDescription.setInputRate(vk::VertexInputRate::eVertex); // @todo Change to eInstance for instance data
        return bindingDescription;
    }

    std::array<vk::VertexInputAttributeDescription, 5> GetVertexAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 5> attributeDescriptions{};
        attributeDescriptions[0].setBinding(0);
        attributeDescriptions[0].setLocation(0);
        attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[0].setOffset(offsetof(Vertex, Position));

        attributeDescriptions[1].setBinding(0);
        attributeDescriptions[1].setLocation(1);
        attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[1].setOffset(offsetof(Vertex, Normal));

        attributeDescriptions[2].setBinding(0);
        attributeDescriptions[2].setLocation(2);
        attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
        attributeDescriptions[2].setOffset(offsetof(Vertex, UV));

        attributeDescriptions[3].setBinding(0);
        attributeDescriptions[3].setLocation(3);
        attributeDescriptions[3].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[3].setOffset(offsetof(Vertex, Tangent));

        attributeDescriptions[4].setBinding(0);
        attributeDescriptions[4].setLocation(4);
        attributeDescriptions[4].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[4].setOffset(offsetof(Vertex, Bitangent));
        return attributeDescriptions;
    }

    MeshManager::MeshManager(nc::graphics::Graphics2* graphics)
    : m_vertexBuffer{std::make_unique<ImmutableBuffer<Vertex>>(graphics)},
      m_indexBuffer{std::make_unique<ImmutableBuffer<uint32_t>>(graphics)},
      m_vertices{},
      m_indices{},
      m_meshes{}
    {
        impl = this;
    }

    void LoadMeshes(const std::vector<std::string>& meshPaths)
    {
        IF_THROW(!impl, "graphics::vulkan::LoadMeshAsset - impl is not set");
        impl->LoadMeshes(meshPaths);
    }

    void MeshManager::LoadMeshes(const std::vector<std::string>& meshPaths)
    {
        for (auto& path : meshPaths)
        {
            if (MeshExists(path)) continue;

            if(!HasValidAssetExtension(path))
                throw std::runtime_error("LoadMeshAsset - Invalid extension: " + path);

            std::ifstream file{path};
            if(!file.is_open())
                throw std::runtime_error("LoadMeshAsset - Could not open file: " + path);

            size_t vertexCount = 0;
            file >> vertexCount;
            std::vector<Vertex> vertices;
            vertices.reserve(vertexCount);
            ReadVerticesFromAsset(&file, &vertices, vertexCount);

            size_t indexCount = 0;
            file >> indexCount;
            std::vector<uint32_t> indices;
            indices.reserve(indexCount);
            ReadIndicesFromAsset(&file, &indices, indexCount);

            auto mesh = Mesh
            {
                path,
                static_cast<uint32_t>(m_vertices.size()), 
                static_cast<uint32_t>(m_indices.size()), 
                static_cast<uint32_t>(indices.size()),
            };

            m_meshes.emplace(path, mesh);
            m_vertices.insert(std::end(m_vertices), std::begin(vertices), std::end(vertices));
            m_indices.insert(std::end(m_indices), std::begin(indices), std::end(indices));
        }

        m_vertexBuffer->Bind(m_vertices);
        m_indexBuffer->Bind(m_indices);
    }

    Mesh MeshManager::GetMesh(const std::string& uid) const
    {
        return m_meshes.at(uid);
    }

    vk::Buffer* MeshManager::GetVertexBuffer()
    {
        return m_vertexBuffer->GetBuffer();
    }

    vk::Buffer* MeshManager::GetIndexBuffer()
    {
        return m_indexBuffer->GetBuffer();
    }

    bool MeshManager::MeshExists(const std::string& uid) const
    {
        return m_meshes.find(uid) != m_meshes.end();
    }

    void MeshManager::Clear()
    {
        m_vertexBuffer.reset();
        m_indexBuffer.reset();
        m_vertices.resize(0);
        m_indices.resize(0);
        m_meshes.clear();
    }
}