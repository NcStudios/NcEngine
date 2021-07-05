#include "DebugTechnique.h"
#include "graphics/TechniqueType.h"
#include "graphics/MaterialProperties.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ShaderResources.h"
#include "graphics/d3dresource/MeshResources.h"
#include "config/Config.h"

namespace nc::graphics
{
    using namespace nc::graphics::d3dresource;

    const auto DebugInputElementDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    const std::string BlenderTag = "texture_blender_resource";


    std::vector<d3dresource::GraphicsResource*> DebugTechnique::m_commonResources;
    std::unique_ptr<graphics::Mesh> DebugTechnique::m_lineMesh;
    std::unique_ptr<graphics::Mesh> DebugTechnique::m_pointMesh;

    DebugTechnique::DebugTechnique()
    {
        DebugTechnique::InitializeCommonResources();

        Step single(3);
        AddStep(std::move(single));
    }

    void DebugTechnique::InitializeCommonResources()
    {
        static bool isInitialized = false;
        if(isInitialized)
            return;
        
        isInitialized = true;

        DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Stencil>(Stencil::GetUID(Stencil::Mode::Off), Stencil::Mode::Off));
        DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Rasterizer>(Rasterizer::GetUID(Rasterizer::Mode::Wireframe), Rasterizer::Mode::Wireframe));

        // Mesh
        //graphics::LoadMeshAsset("project/assets/mesh/plane.nca");
        //DebugTechnique::m_mesh = std::make_unique<graphics::Mesh>("project/assets/mesh/plane.nca");

        // Textures
        //const std::string particleTexturePath = "nc//resources//texture//DefaultParticle.png";
        //const std::string normalPath = "nc/resources/texture/DefaultNormal.png";
        //const std::string roughnessPath = "nc/resources/texture/DefaultMetallic.png";
        //const std::string metallicPath = "nc/resources/texture/DefaultMetallic.png";
        //DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(particleTexturePath), particleTexturePath, 0));
        //DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(normalPath), normalPath, 1));
        //DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(roughnessPath), roughnessPath, 2));
        //DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(metallicPath), metallicPath, 3));

        // Material Properties
        //MaterialProperties materialProperties{};
        //m_materialPropertiesBuffer = std::make_unique<PixelConstantBuffer<MaterialProperties>>(materialProperties, 1u);
        //DebugTechnique::m_commonResources.push_back(m_materialPropertiesBuffer.get());

        // Add vertex shader
        const auto defaultShaderPath = nc::config::GetGraphicsSettings().d3dShadersPath;
        const auto vertexShaderPath = defaultShaderPath + "wireframevertexshader.cso";
        auto pvs = GraphicsResourceManager::AcquireOnDemand<VertexShader>(VertexShader::GetUID(vertexShaderPath), vertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        DebugTechnique::m_commonResources.push_back(std::move(pvs));
        DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<InputLayout>(InputLayout::GetUID("wireframevertexshader"), DebugInputElementDesc, pvsbc));

        // Add pixel shader
        const auto pixelShaderPath = defaultShaderPath + "wireframepixelshader.cso";
        DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<PixelShader>(PixelShader::GetUID(pixelShaderPath), pixelShaderPath));
        DebugTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Blender>(Blender::GetUID(BlenderTag), true));
    }

    void DebugTechnique::BindCommonResources()
    {
        for(const auto& res : DebugTechnique::m_commonResources)
        {
            res->Bind();
        }
    }

}