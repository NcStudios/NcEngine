#pragma once

#include "ResourceGroup.h"
#include "directx/math/DirectXMath.h"

#include <string>

namespace nc::graphics
{
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

    class PBRMaterial : public ResourceGroup
    {
        public: 
            PBRMaterial() = default;
            PBRMaterial(const std::vector<std::string>& texturePaths);
            MaterialProperties properties;

        private:
            void InitializeGraphicsPipeline(const std::vector<std::string>& texturePaths);
    };
}