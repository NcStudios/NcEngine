#include "graphics/MeshManager.h"
#include "graphics/Graphics.h"
#include "graphics/resources/ResourceManager.h"
#include "graphics/resources/GraphicsResources.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "debug/Utils.h"

#include <fstream>

namespace
{
    using namespace nc::graphics;
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

namespace nc::graphics
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

    MeshManager::MeshManager(nc::graphics::Graphics* graphics)
    : m_graphics{graphics}
    {
        impl = this;
    }

    void LoadMeshes(const std::vector<std::string>& meshPaths)
    {
        IF_THROW(!impl, "graphics::LoadMeshes - impl is not set");
        impl->LoadMeshes(meshPaths);
    }

    void MeshManager::LoadMeshes(const std::vector<std::string>& meshPaths)
    {
        std::vector<Vertex> allVertices = {};
        std::vector<uint32_t> allIndices = {};
        std::unordered_map<std::string, Mesh> meshes;
        std::vector<std::string> pathsToLoad;

        auto meshesExist = ResourceManager::HasMeshes();
        if (meshesExist)
        {
            pathsToLoad = ResourceManager::GetMeshPaths();
            pathsToLoad.insert(meshPaths.end(), meshPaths.begin(), meshPaths.end());
        }
        else
        {
            pathsToLoad = meshPaths;
        }

        for (auto& path : pathsToLoad)
        {
            if (ResourceManager::MeshExists(path)) continue;

            if(!HasValidAssetExtension(path))
                throw std::runtime_error("LoadMeshes - Invalid extension: " + path);

            std::ifstream file{path};
            if(!file.is_open())
                throw std::runtime_error("LoadMeshes - Could not open file: " + path);

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

            auto mesh = graphics::Mesh
            {
                static_cast<uint32_t>(allVertices.size()), 
                static_cast<uint32_t>(allIndices.size()), 
                static_cast<uint32_t>(indices.size()),
            };

            meshes.emplace(path, mesh);
            allVertices.insert(std::end(allVertices), std::begin(vertices), std::end(vertices));
            allIndices.insert(std::end(allIndices), std::begin(indices), std::end(indices));
        }

        auto vertexBuffer = ImmutableBuffer<Vertex>(m_graphics, std::move(allVertices));
        auto indexBuffer = ImmutableBuffer<uint32_t>(m_graphics, std::move(allIndices));
        auto meshesData = std::make_unique<MeshesData>(std::move(vertexBuffer), std::move(indexBuffer), std::move(meshes));
        ResourceManager::LoadMeshes(std::move(meshesData));
    }
}