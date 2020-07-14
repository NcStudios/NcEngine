#include "NcDebug.h"
#include "graphics\d3dresource\GraphicsResourceManager.h"
#include "graphics\Mesh.h"
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
           throw NcException("nc::graphics::detail::GetMeshFileName::HasValidMeshExtension - Invalid file type loaded for mesh.");
        }

        return fileNameWithExtension.substr(0, periodPosition);
    } 
}

namespace nc::graphics
{
    Mesh::Mesh(std::string meshPath)
    : m_meshData {}
    {
        m_meshData.MeshPath = std::move(meshPath);

        struct Vertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT3 normal;
            DirectX::XMFLOAT2 uvs;
        };

        Assimp::Importer imp;
        const auto pModel = imp.ReadFile(m_meshData.MeshPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded); // ConvertToLeftHanded formats the output to match DirectX
        const auto pMesh = pModel->mMeshes[0];

        // Load vertex and normal data
        m_meshData.Vertices.reserve(pMesh -> mNumVertices);
        for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
        {
            m_meshData.Vertices.push_back( {
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mVertices[i]),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]),
                *reinterpret_cast<DirectX::XMFLOAT2*>(&pMesh->mTextureCoords[0][i])
            } );
        }

        // Load index data
        std::vector<unsigned short> indices;
        m_meshData.Indices.reserve(pMesh -> mNumFaces * 3); // Multiply by 3 because we told assimp to triangulate (aiProcess_Triangulate). Each face has 3 indices
        for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
        {
            const auto& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            m_meshData.Indices.push_back(face.mIndices[0]);
            m_meshData.Indices.push_back(face.mIndices[1]);
            m_meshData.Indices.push_back(face.mIndices[2]);
        }

        m_meshData.Name = detail::GetMeshFileName(m_meshData.MeshPath);
        m_meshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        m_meshData.InputElementDesc = 
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
    }

    MeshData& Mesh::GetMeshData() 
    {
        return m_meshData;
    }
}