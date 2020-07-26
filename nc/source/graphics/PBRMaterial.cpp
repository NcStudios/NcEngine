#include "PBRMaterial.h"
#include "NcConfig.h"
#include "config/Config.h"

namespace nc::graphics
{
    PBRMaterial::PBRMaterial(const std::string& albedoTexturePath, 
                             const std::string& normalTexturePath, 
                             const std::string& metallicTexturePath, 
                             const std::string& roughnessTexturePath)
                             : properties {},
                               m_texturePaths { albedoTexturePath, normalTexturePath, metallicTexturePath, roughnessTexturePath }
                               

    {
        const auto defaultShaderPath = nc::config::NcGetConfigReference().graphics.shadersPath;
        m_vertexShaderPath = defaultShaderPath + "pbrvertexshader.cso";
        m_pixelShaderPath = defaultShaderPath + "pbrpixelshader.cso";
    }

    const std::vector<std::string>& PBRMaterial::GetTexturePaths() const
    {
        return m_texturePaths;
    }

    const std::string& PBRMaterial::GetVertexShaderPath() const
    {
        return m_vertexShaderPath;
    }

    const std::string& PBRMaterial::GetPixelShaderPath() const
    {
        return m_pixelShaderPath;
    }
}