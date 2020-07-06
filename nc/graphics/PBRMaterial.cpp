#include "PBRMaterial.h"
#include "Graphics.h"
#include "d3dresource/GraphicsResourceManager.h"
#include "d3dresource/GraphicsResource.h"

namespace nc::graphics
{
    PBRMaterial::PBRMaterial(const std::string& albedoTexturePath, const std::string& normalTexturePath, const std::string& metallicTexturePath, const std::string& roughnessTexturePath) 
    {
        using namespace d3dresource;

        AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>(albedoTexturePath));
        //AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>(normalTexturePath));
        //AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>(metallicTexturePath));
        //AddGraphicsResource(GraphicsResourceManager::Acquire<Texture>(roughnessTexturePath));
        AddGraphicsResource(GraphicsResourceManager::Acquire<Sampler>("PBRSampler"));

        // Shaders

    }

    void PBRMaterial::AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res)
    {
        Resources.push_back(std::move(res));
    }
}