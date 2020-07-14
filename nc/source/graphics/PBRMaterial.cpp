#include "PBRMaterial.h"
#include "NcConfig.h"
#include "config/Config.h"

namespace nc::graphics
{
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    PBRMaterial::PBRMaterial(const DirectX::XMFLOAT3& diffuseColor,
                    const std::string& albedoTexturePath, 
                    const std::string& normalTexturePath, 
                    const std::string& metallicTexturePath, 
                    const std::string& roughnessTexturePath)
                    : m_diffuseColor { diffuseColor },
                      m_texturePaths { albedoTexturePath, normalTexturePath, metallicTexturePath, roughnessTexturePath }

    {
        const auto defaultShaderPath = nc::config::NcGetConfigReference().graphics.shadersPath;
        m_vertexShaderPath = defaultShaderPath + "pbrvertexshader.cso";
        m_pixelShaderPath = defaultShaderPath + "pbrpixelshader.cso";
    }

    const DirectX::XMFLOAT3& PBRMaterial::GetDiffuseColor() const
    {
        return m_diffuseColor;
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