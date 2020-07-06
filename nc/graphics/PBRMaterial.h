#pragma once

#include <string>
#include <memory>
#include <vector>
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    namespace d3dresource { class GraphicsResource; }

    class PBRMaterial
    {
        public:
            PBRMaterial(const std::string& albedoTexturePath = "D:\\NC\\NCEngine\\nc\\graphics\\DefaultTexture.png", const std::string& normalTexturePath = "D:\\NC\\NCEngine\\nc\\graphics\\DefaultTexture.png", const std::string& metallicTexturePath = "D:\\NC\\NCEngine\\nc\\graphics\\DefaultTexture.png", const std::string& roughnessTexturePath = "D:\\NC\\NCEngine\\nc\\graphics\\DefaultTexture.png");
            std::vector<std::shared_ptr<d3dresource::GraphicsResource>> Resources;

        private:
            void AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res);
    };
}