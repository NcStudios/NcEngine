#include "PBRMaterial.h"
#include "NcConfig.h"
#include "config/Config.h"
#include "d3dresource/GraphicsResourceManager.h"

namespace nc::graphics
{
    PBRMaterial::PBRMaterial(const std::vector<std::string>& texturePaths)
                            : properties{}                             
    {
        PBRMaterial::InitializeGraphicsPipeline(texturePaths);
    }

    void PBRMaterial::InitializeGraphicsPipeline(const std::vector<std::string>& texturePaths)
    {
        using namespace nc::graphics::d3dresource;

        uint32_t shaderIndex = 0;
        for (const auto& texturePathRef : texturePaths) 
        {
            PBRMaterial::AddGraphicsResource(d3dresource::GraphicsResourceManager::Acquire<d3dresource::Texture>(texturePathRef, shaderIndex++));
        }

        const auto defaultShaderPath = nc::config::NcGetConfigReference().graphics.shadersPath;
        PBRMaterial::AddGraphicsResource(d3dresource::GraphicsResourceManager::Acquire<d3dresource::PixelShader>(defaultShaderPath + "pbrpixelshader.cso"));
        auto samplerId = std::to_string(GraphicsResourceManager::AssignId());
        PBRMaterial::AddGraphicsResource(d3dresource::GraphicsResourceManager::Acquire<d3dresource::Sampler>(samplerId));
        PBRMaterial::AddGraphicsResource(PixelConstantBuffer<MaterialProperties>::AcquireUnique(properties, 1u));
    }
}