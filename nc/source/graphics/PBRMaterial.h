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
        const std::string DefaultTexturePath = "nc\\graphics\\DefaultTexture.png";
    }

    struct MaterialProperties
    {
        DirectX::XMFLOAT3 color = {0.25f, 0.25f, 0.25f};
        float specularIntensity = 0.6;
        float specularPower = 32.0f;
        float xTiling = 1.0f;
        float yTiling = 1.0f;
        float padding[1];
    };

    class PBRMaterial
    {
        public: 

        PBRMaterial(const std::string& albedoTexturePath = detail::DefaultTexturePath, 
                    const std::string& normalTexturePath = detail::DefaultTexturePath, 
                    const std::string& metallicTexturePath = detail::DefaultTexturePath, 
                    const std::string& roughnessTexturePath = detail::DefaultTexturePath);
        
        const std::string& GetVertexShaderPath() const;
        const std::string& GetPixelShaderPath() const;
        const std::vector<std::string>& GetTexturePaths() const;
        MaterialProperties properties;

        private:
            std::vector<std::string> m_texturePaths;
            std::string m_pixelShaderPath;
            std::string m_vertexShaderPath;
    };
}