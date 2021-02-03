#include "graphics\Mesh.h"
#include "graphics\d3dresource\GraphicsResourceManager.h"
#include "graphics\d3dresource\MeshResources.h"
#include "DebugUtils.h"
#include "Vertex.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

const char separator = 
#ifdef _WIN32
    '\\';
#else 
    '/';
#endif

namespace
{
    constexpr auto AssimpFlags = aiProcess_Triangulate |
                                  aiProcess_JoinIdenticalVertices |
                                  aiProcess_ConvertToLeftHanded |
                                  aiProcess_GenNormals |
                                  aiProcess_CalcTangentSpace;

    constexpr auto DefaultPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    bool HasValidMeshExtension(const std::string& path) 
    {
        std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);

        if (fileExtension.compare("fbx") == 0 || fileExtension.compare("FBX") == 0)
        {
            return true;
        }
        if (fileExtension.compare("obj") == 0 || fileExtension.compare("OBJ") == 0)
        {
            return true;
        }
        return false;
    }

    void ParseMesh(std::string meshPath, std::vector<nc::graphics::Vertex>& vBuffOut, std::vector<uint16_t>& iBuffOut)
    {
        if (!HasValidMeshExtension(meshPath))
        {
            throw std::runtime_error("Invalid mesh file extension");
        }

        Assimp::Importer imp;
        const auto pModel = imp.ReadFile(meshPath, AssimpFlags);
        const auto pMesh = pModel->mMeshes[0];

        // Load vertex and normal data
        vBuffOut.reserve(pMesh -> mNumVertices);
        for (size_t i = 0; i < pMesh->mNumVertices; i++)
        {
            vBuffOut.push_back( {
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mVertices[i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]),
                *reinterpret_cast<DirectX::XMFLOAT2*>(&pMesh->mTextureCoords[0][i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mTangents[i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mBitangents[i])
            } );
        }

        // Load index data
        iBuffOut.reserve(pMesh -> mNumFaces * 3); // Multiply by 3 because we told assimp to triangulate (aiProcess_Triangulate). Each face has 3 indices
        for (size_t i = 0; i < pMesh->mNumFaces; i++)
        {
            const auto& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            iBuffOut.push_back(face.mIndices[0]);
            iBuffOut.push_back(face.mIndices[1]);
            iBuffOut.push_back(face.mIndices[2]);
        }
    }
} // end anonymous namespace

namespace nc::graphics
{
    using namespace nc::graphics::d3dresource;

    Mesh::Mesh(std::string meshPath)
    {
        AddBufferResources(std::move(meshPath));
    }

    void Mesh::AddBufferResources(std::string meshPath)
    {
        // Topology does not depend on parsing the mesh
        AddGraphicsResource(GraphicsResourceManager::Acquire<Topology> (DefaultPrimitiveTopology));

        auto vertexBuffer = std::vector<Vertex>{};
        auto indexBuffer = std::vector<uint16_t>{};

        auto vBufferExists = GraphicsResourceManager::Exists<VertexBuffer>(vertexBuffer, meshPath);
        auto iBufferExists = GraphicsResourceManager::Exists<IndexBuffer>(indexBuffer, meshPath);

        if (!(vBufferExists && iBufferExists))
        {
            ParseMesh(meshPath, vertexBuffer, indexBuffer);
        }

        // If the resource already exists, we can safely pass in empty buffers
        AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(vertexBuffer, meshPath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer>(indexBuffer, meshPath));
    }
} // end namespace nc::graphics