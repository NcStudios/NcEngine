#include "PhongShadingTechnique.h"
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
    const auto PhongInputElementDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>
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
    std::vector<GraphicsResource*> PhongShadingTechnique::m_commonResources = {};

    PhongShadingTechnique::PhongShadingTechnique(const std::vector<std::string>& texturePaths, MaterialProperties materialProperties)
        : m_materialProperties {materialProperties}
    {
        PhongShadingTechnique::InitializeCommonResources();
        
        Step single(0);
        {
            // Add Textures
            uint32_t shaderIndex = 0;
            for (const auto& texturePathRef : texturePaths) 
            {
                single.AddGraphicsResource(GraphicsResourceManager::AcquireOnDemand<Texture>(Texture::GetUID(texturePathRef), texturePathRef, shaderIndex++));
            }

            m_materialPropertiesBuffer = std::make_unique<PixelConstantBuffer<MaterialProperties>>(materialProperties, 1u);
            single.AddGraphicsResource(m_materialPropertiesBuffer.get());
        }
        AddStep(std::move(single));
    }

    #ifdef NC_EDITOR_ENABLED
    void PhongShadingTechnique::EditorGuiElement()
    {
        const float dragSpeed = 0.1f;
        ImGui::SameLine();
        ImGui::Text("(Phong Shading)");
        ImGui::Spacing();
        ImGui::Indent();
            ImGui::Text("Material Color"); ImGui::SameLine(); bool mcDirty = ImGui::ColorEdit3("##mc", &(m_materialProperties.color.x), ImGuiColorEditFlags_NoInputs);
            ImGui::Text("Specular");
            ImGui::Indent();
                bool siDirty = ui::editor::floatWidget("Intensity", "specintensity", &m_materialProperties.specularIntensity, dragSpeed,  0.05f, 4.0f, "%.2f");
                bool spDirty = ui::editor::floatWidget("Power    ", "specpower", &m_materialProperties.specularPower, dragSpeed,  0.5f, 13.0f, "%.2f");
            ImGui::Unindent();
            bool txDirty = ui::editor::floatWidget("X Tiling", "tilex", &m_materialProperties.xTiling, dragSpeed, 0.001f, 100.0f, "%.2f");
            bool tyDirty = ui::editor::floatWidget("Y Tiling", "tiley", &m_materialProperties.yTiling, dragSpeed, 0.001f, 100.0f, "%.2f");
        ImGui::Unindent();

        if(mcDirty || siDirty || spDirty || txDirty || tyDirty)
        {
            m_materialPropertiesBuffer->Update(m_materialProperties);
        }
    }
    
    #endif

    size_t PhongShadingTechnique::GetUID(const std::vector<std::string>& texturePaths, const MaterialProperties& materialProperties) noexcept
    {
        auto hash = std::to_string((uint8_t)TechniqueType::PhongShading);
        for (const auto& texturePathRef : texturePaths) 
        {
            hash += texturePathRef;
        }

        /** @todo what is the purpose of this? */
        hash += std::to_string(materialProperties.color.x);
        return std::hash<std::string>{}(hash);
    }

    void PhongShadingTechnique::InitializeCommonResources()
    {
        static bool isInitialized = false;
        if(isInitialized)
        {
            return;
        }

        isInitialized = true;

        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Stencil>(Stencil::GetUID(Stencil::Mode::Off), Stencil::Mode::Off));
        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Rasterizer>(Rasterizer::GetUID(Rasterizer::Mode::Solid), Rasterizer::Mode::Solid));

        // Add vertex shader
        const auto defaultShaderPath = nc::config::GetGraphicsSettings().d3dShadersPath;
        const auto vertexShaderPath = defaultShaderPath + "phongvertexshader.cso";
        auto pvs = GraphicsResourceManager::AcquireOnDemand<VertexShader>(VertexShader::GetUID(vertexShaderPath), vertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        PhongShadingTechnique::m_commonResources.push_back(std::move(pvs));
        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<InputLayout>(InputLayout::GetUID("phongvertexshader"), PhongInputElementDesc, pvsbc));

        // Add pixel shader
        const auto pixelShaderPath = defaultShaderPath + "phongpixelshader.cso";
        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<PixelShader>(PixelShader::GetUID(pixelShaderPath), pixelShaderPath));
        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Blender>(Blender::GetUID(BLENDER_TAG), false));
        PhongShadingTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Sampler>(Sampler::GetUID(SAMPLER_TAG)));
    }

    void PhongShadingTechnique::BindCommonResources()
    {
        for(const auto& res : PhongShadingTechnique::m_commonResources)
        {
            res->Bind();
        }
    }


}