#include "ParticleTechnique.h"
#include "graphics/TechniqueType.h"
#include "graphics/MaterialProperties.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ShaderResources.h"
#include "graphics/d3dresource/MeshResources.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "config/Config.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

using namespace nc::graphics::d3dresource;

namespace
{
    const auto ParticleInputElementDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    const std::string SamplerTag = "texture_sampler_resource";
    const std::string BlenderTag = "particle_texture_blender_resource";
}

namespace nc::graphics
{
    std::vector<GraphicsResource*> ParticleTechnique::m_commonResources = {};
    std::unique_ptr<graphics::Mesh> ParticleTechnique::m_mesh = nullptr;
    std::unique_ptr<d3dresource::PixelConstantBuffer<MaterialProperties>> ParticleTechnique::m_materialPropertiesBuffer = nullptr;

    ParticleTechnique::ParticleTechnique()
    {
        ParticleTechnique::InitializeCommonResources();
        
        Step single(2);
        {
        }
        AddStep(std::move(single));
    }

    #ifdef NC_EDITOR_ENABLED
    void ParticleTechnique::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("(Particle)");
    }
    #endif

    size_t ParticleTechnique::GetUID() noexcept
    {
        const size_t name = 200;
        return name;
    }

    void ParticleTechnique::InitializeCommonResources()
    {
        static bool isInitialized = false;
        if(isInitialized)
        {
            return;
        }

        isInitialized = true;

        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Stencil>(Stencil::GetUID(Stencil::Mode::Off), Stencil::Mode::Off));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Rasterizer>(Rasterizer::GetUID(Rasterizer::Mode::Solid), Rasterizer::Mode::Solid));

        // Mesh
        graphics::LoadMeshAsset("project/assets/mesh/plane.nca");
        ParticleTechnique::m_mesh = std::make_unique<graphics::Mesh>("project/assets/mesh/plane.nca");

        // Textures
        const std::string particleTexturePath = "nc//resources//texture//DefaultParticle.png";
        const std::string normalPath = "nc/resources/texture/DefaultNormal.png";
        const std::string roughnessPath = "nc/resources/texture/DefaultMetallic.png";
        const std::string metallicPath = "nc/resources/texture/DefaultMetallic.png";
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(particleTexturePath), particleTexturePath, 0));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(normalPath), normalPath, 1));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(roughnessPath), roughnessPath, 2));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(metallicPath), metallicPath, 3));

        // Material Properties
        MaterialProperties materialProperties{};
        m_materialPropertiesBuffer = std::make_unique<PixelConstantBuffer<MaterialProperties>>(materialProperties, 1u);
        ParticleTechnique::m_commonResources.push_back(m_materialPropertiesBuffer.get());

        // Add vertex shader
        const auto defaultShaderPath = nc::config::GetGraphicsSettings().d3dShadersPath;
        const auto vertexShaderPath = defaultShaderPath + "phongvertexshader.cso";
        auto pvs = GraphicsResourceManager::AcquireOnDemand<VertexShader>(VertexShader::GetUID(vertexShaderPath), vertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        ParticleTechnique::m_commonResources.push_back(std::move(pvs));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<InputLayout>(InputLayout::GetUID("phongvertexshader"), ParticleInputElementDesc, pvsbc));

        // Add pixel shader
        const auto pixelShaderPath = defaultShaderPath + "phongpixelshader.cso";
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<PixelShader>(PixelShader::GetUID(pixelShaderPath), pixelShaderPath));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Blender>(Blender::GetUID(BlenderTag), true));
        ParticleTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Sampler>(Sampler::GetUID(SamplerTag)));
    }

    void ParticleTechnique::BindCommonResources()
    {
        m_mesh->Bind();

        for(const auto& res : ParticleTechnique::m_commonResources)
        {
            res->Bind();
        }
    }
}