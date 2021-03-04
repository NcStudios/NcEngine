#include "graphics/Mesh.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/MeshResources.h"
#include "debug/Utils.h"
#include "Vertex.h"

#include <fstream>

namespace
{
    constexpr auto DefaultPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    bool HasValidAssetExtension(const std::string& path) 
    {
        std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension.compare("nca") == 0 ? true : false;
    }

    void ReadVerticesFromAsset(std::ifstream* file, std::vector<nc::graphics::Vertex>* out, size_t count)
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

    void ReadIndicesFromAsset(std::ifstream* file, std::vector<uint16_t>* out, size_t count)
    {
        uint16_t index;
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
    using namespace nc::graphics::d3dresource;

    void LoadMeshAsset(const std::string& path)
    {
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
        if(!GraphicsResourceManager::Load<VertexBuffer>(VertexBuffer::GetUID(path), vertices))
            throw std::runtime_error("LoadMeshAsset - Failed to load vertex buffer resource");

        size_t indexCount = 0;
        file >> indexCount;
        std::vector<uint16_t> indices;
        indices.reserve(indexCount);
        ReadIndicesFromAsset(&file, &indices, indexCount);
        if(!GraphicsResourceManager::Load<IndexBuffer>(IndexBuffer::GetUID(path), indices))
            throw std::runtime_error("LoadMeshAsset - Failed to load index buffer resource");
    }

    Mesh::Mesh(const std::string& meshPath)
    {
        AddGraphicsResource(GraphicsResourceManager::AcquireOnDemand<Topology>(Topology::GetUID(DefaultPrimitiveTopology), DefaultPrimitiveTopology));

        // We are requiring v/i buffers to be loaded prior to creating any meshes.
        auto vBufPtr = GraphicsResourceManager::Acquire(VertexBuffer::GetUID(meshPath));
        auto iBufPtr = GraphicsResourceManager::Acquire(IndexBuffer::GetUID(meshPath));

        if(!vBufPtr || !iBufPtr)
            throw std::runtime_error("Mesh::AddBufferResources - Failed to acquire resources. Are they loaded?: " + meshPath);
    
        AddGraphicsResource(vBufPtr);
        AddGraphicsResource(iBufPtr);
    }
} // end namespace nc::graphics