#pragma once

#include <string>
#include <memory>
#include <vector>
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    namespace d3dresource { class GraphicsResource; }

    namespace detail 
    {
        const DirectX::XMFLOAT3 DefaultColor = {1.0f, 1.0f, 1.0f};
        const std::string DefaultTexturePath = "nc\\graphics\\DefaultTexture.png";
    }

    class PBRMaterial
    {
        public: 

        PBRMaterial(const DirectX::XMFLOAT3& diffuseColor = detail::DefaultColor,
                    const std::string& albedoTexturePath = detail::DefaultTexturePath, 
                    const std::string& normalTexturePath = detail::DefaultTexturePath, 
                    const std::string& metallicTexturePath = detail::DefaultTexturePath, 
                    const std::string& roughnessTexturePath = detail::DefaultTexturePath);
        
            const DirectX::XMFLOAT3& GetDiffuseColor() const;
            const std::string& GetVertexShaderPath() const;
            const std::string& GetPixelShaderPath() const;
            const std::vector<std::string>& GetTexturePaths() const;

        private:
        
            DirectX::XMFLOAT3 m_diffuseColor;
            std::vector<std::string> m_texturePaths;
            std::string m_pixelShaderPath;
            std::string m_vertexShaderPath;
    };
}