#include "Debug\NCException.h"
#include "GraphicsResourceManager.h"
#include "Mesh.h"
#include "ObjLoader.h"

const char separator = 
#ifdef _WIN32
    '\\';
#else 
    '/';
#endif

namespace nc::graphics
{
    Mesh::Mesh(Graphics * graphics, std::string meshPath)
    : m_meshData {}
    {
        m_meshData.MeshPath = std::move(meshPath);
        utils::ObjLoader loader;
        loader.Load(&m_meshData);
        m_meshData.Name = detail::GetMeshFileName(meshPath);
        m_meshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        m_meshData.InputElementDesc = 
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
    }

    MeshData& Mesh::GetMeshData() 
    {
        return m_meshData;
    }
}

namespace nc::graphics::detail
{
    std::string GetMeshFileName(std::string meshPath) 
    {
        std::size_t fileNameStartPosition = meshPath.rfind(separator);
        std::string fileNameWithExtension = meshPath.substr(fileNameStartPosition+1);
        std::size_t periodPosition = fileNameWithExtension.find('.');
        std::string fileExtension = fileNameWithExtension.substr(periodPosition+1);

        if (!HasValidMeshExtension(fileExtension)) 
        {
           throw DefaultException("nc::graphics::detail::GetMeshFileName::HasValidMeshExtension - Invalid file type loaded for mesh.");
        }

        return fileNameWithExtension.substr(0, periodPosition);

    } 

    bool HasValidMeshExtension(std::string fileExtension) 
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
}