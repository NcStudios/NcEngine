#include "NcDebug.h"
#include "graphics\d3dresource\GraphicsResourceManager.h"
#include "graphics\Mesh.h"
#include "NcConfig.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <vector>
#include "external\include\directx\math\DirectXMath.h"

const char separator = 
#ifdef _WIN32
    '\\';
#else 
    '/';
#endif


namespace nc::graphics::detail
{
    bool HasValidMeshExtension(const std::string& fileExtension) 
    {
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

    std::string GetMeshFileName(const std::string& meshPath) 
    {
        std::size_t fileNameStartPosition = meshPath.rfind(separator);
        const std::string& fileNameWithExtension = meshPath.substr(fileNameStartPosition+1);
        std::size_t periodPosition = fileNameWithExtension.find('.');
        const std::string& fileExtension = fileNameWithExtension.substr(periodPosition+1);

        if (!HasValidMeshExtension(fileExtension)) 
        {
           throw std::runtime_error("nc::graphics::detail::GetMeshFileName::HasValidMeshExtension - Invalid file type loaded for mesh.");
        }

        return fileNameWithExtension.substr(0, periodPosition);
    } 
}

namespace nc::graphics
{
    Mesh::Mesh(std::string meshPath)
    {
        InitializeGraphicsPipeline(Mesh::ParseMesh(std::move(meshPath)));
    }

    MeshData Mesh::ParseMesh(std::string meshPath) 
    {
        MeshData meshData;
        meshData.MeshPath = std::move(meshPath);
        Assimp::Importer imp;
        const auto pModel = imp.ReadFile(meshData.MeshPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace); // ConvertToLeftHanded formats the output to match DirectX
        const auto pMesh = pModel->mMeshes[0];

        // Load vertex and normal data
        meshData.Vertices.reserve(pMesh -> mNumVertices);
        for (size_t i = 0; i < pMesh->mNumVertices; i++)
        {
            meshData.Vertices.push_back( {
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mVertices[i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]),
                *reinterpret_cast<DirectX::XMFLOAT2*>(&pMesh->mTextureCoords[0][i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mTangents[i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mBitangents[i])
            } );
        }

        // Load index data
        meshData.Indices.reserve(pMesh -> mNumFaces * 3); // Multiply by 3 because we told assimp to triangulate (aiProcess_Triangulate). Each face has 3 indices
        for (size_t i = 0; i < pMesh->mNumFaces; i++)
        {
            const auto& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            meshData.Indices.push_back(face.mIndices[0]);
            meshData.Indices.push_back(face.mIndices[1]);
            meshData.Indices.push_back(face.mIndices[2]);
        }

        meshData.Name = detail::GetMeshFileName(meshData.MeshPath);
        meshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        meshData.InputElementDesc = 
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        return meshData;
    }

    void Mesh::InitializeGraphicsPipeline(MeshData meshData)
    {
        using namespace nc::graphics::d3dresource;

        auto defaultShaderPath = nc::config::NcGetConfigReference().graphics.shadersPath;

        auto pvs = d3dresource::GraphicsResourceManager::Acquire<d3dresource::VertexShader>(std::move(defaultShaderPath) + "pbrvertexshader.cso");
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        Mesh::AddGraphicsResource(std::move(pvs));
        Mesh::AddGraphicsResource(GraphicsResourceManager::Acquire<VertexBuffer>(meshData.Vertices, meshData.MeshPath));
        Mesh::AddGraphicsResource(GraphicsResourceManager::Acquire<IndexBuffer> (meshData.Indices, meshData.MeshPath));
        Mesh::AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout> (meshData.MeshPath, meshData.InputElementDesc, pvsbc));
        Mesh::AddGraphicsResource(GraphicsResourceManager::Acquire<Topology>    (meshData.PrimitiveTopology));
    }
}