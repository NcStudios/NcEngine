#include "PBRMaterial.h"
#include "graphics/rendergraph/Step.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "Engine.h"

using namespace nc::graphics::d3dresource;

namespace
{
    const auto PbrInputElementDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    const std::string SAMPLER_TAG = "texture_sampler_resource";
    const std::string BLENDER_TAG = "texture_blender_resource";
}

namespace nc::graphics
{
    PBRMaterial::PBRMaterial(const std::vector<std::string>& texturePaths)
    {
        auto defaultShaderPath = nc::engine::Engine::GetConfig().graphics.shadersPath;

        Technique pbrTechnique;
        {
            Step single(0);
            {
                // Add Textures
                uint32_t shaderIndex = 0;
                for (const auto& texturePathRef : texturePaths) 
                {
                    single.AddGraphicsResource(d3dresource::GraphicsResourceManager::Acquire<d3dresource::Texture>(texturePathRef, shaderIndex++));
                }

                // Add vertex shader
                auto pvs = GraphicsResourceManager::Acquire<VertexShader>(defaultShaderPath + "pbrvertexshader.cso");
                auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
                single.AddGraphicsResource(std::move(pvs));
                single.AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout>("pbrvertexshader", PbrInputElementDesc, pvsbc));

                // Add pixel shader
                single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::PixelShader>(defaultShaderPath + "pbrpixelshader.cso"));
                single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::Blender>(BLENDER_TAG));
                single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::Sampler>(SAMPLER_TAG));
                single.AddGraphicsResource(PixelConstantBuffer<MaterialProperties>::AcquireUnique(properties, 1u));
            }
            pbrTechnique.AddStep(std::move(single));
        }
        AddTechnique(std::move(pbrTechnique));
    }


}