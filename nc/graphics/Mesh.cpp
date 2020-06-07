#include "Debug\NCException.h"
#include "graphics\d3dresource\GraphicsResourceManager.h"
#include "graphics\Mesh.h"
#include "utils\objloader\ObjLoader.h"

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
           throw DefaultException("nc::graphics::detail::GetMeshFileName::HasValidMeshExtension - Invalid file type loaded for mesh.");
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
        utils::ObjLoader loader;
        loader.Load(&m_meshData);
        m_meshData.Name = detail::GetMeshFileName(m_meshData.MeshPath);
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