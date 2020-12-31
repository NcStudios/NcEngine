#include "PhongShadingTechnique.h"
#include "TechniqueType.h"
#include "graphics/materials/MaterialProperties.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ShaderResources.h"
#include "graphics/d3dresource/MeshResources.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "Engine.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
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
    PhongShadingTechnique::PhongShadingTechnique(const std::vector<std::string>& texturePaths, MaterialProperties materialProperties)
    : m_materialProperties {materialProperties}
    {
        auto defaultShaderPath = nc::engine::Engine::GetConfig().graphics.shadersPath;

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
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout>("pbrvertexshader", PhongInputElementDesc, pvsbc));

            // Add pixel shader
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::PixelShader>(defaultShaderPath + "pbrpixelshader.cso"));
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::Blender>(BLENDER_TAG));
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::Sampler>(SAMPLER_TAG));
            m_materialPropertiesBuffer = std::make_unique<PixelConstantBuffer<MaterialProperties>>(materialProperties, 1u);
            single.AddGraphicsResource(m_materialPropertiesBuffer.get());
        }
        AddStep(std::move(single));
    }

    #ifdef NC_EDITOR_ENABLED
    void PhongShadingTechnique::EditorGuiElement()
    {
        const float dragSpeed = 1.0f;
        ImGui::SameLine();
        ImGui::Text("(Phong Shading)");
        ImGui::PushItemWidth(80.0f);
            ImGui::Spacing();
            ImGui::Indent();
                ImGui::Text("Material Color"); ImGui::SameLine(); bool mcDirty = ImGui::ColorEdit3("##mc", &(m_materialProperties.color.x), ImGuiColorEditFlags_NoInputs);
                ImGui::Text("Specular");
                ImGui::Indent();
                ImGui::Text("Intensity"); ImGui::SameLine(); bool siDirty = ImGui::SliderFloat("##si", &(m_materialProperties.specularIntensity), 0.05f, 4.0f, "%.2f", dragSpeed);
                ImGui::Text("Power    "); ImGui::SameLine(); bool spDirty = ImGui::SliderFloat("##sp", &(m_materialProperties.specularPower), 0.5f, 13.0f, "%.2f", dragSpeed);
                ImGui::Unindent();
                ImGui::Text("Tiling X"); ImGui::SameLine(); bool txDirty = ImGui::SliderFloat("##tx", &(m_materialProperties.xTiling), 0.001f, 100.0f, "%.2f", dragSpeed);
                ImGui::Text("Tiling Y"); ImGui::SameLine(); bool tyDirty = ImGui::SliderFloat("##ty", &(m_materialProperties.yTiling), 0.001f, 100.0f, "%.2f", dragSpeed);
            ImGui::Unindent();
        ImGui::PopItemWidth();

        if(mcDirty || siDirty || spDirty || txDirty || tyDirty)
        {
            m_materialPropertiesBuffer->Update(m_materialProperties);

        }
    }
    
    #endif

    size_t PhongShadingTechnique::GetUID(TechniqueType type, const std::vector<std::string>& texturePaths) noexcept
    {
        auto hash = std::to_string((uint8_t)type);
        for (const auto& texturePathRef : texturePaths) 
        {
            hash += texturePathRef;
        }
        return std::hash<std::string>{}(hash);
    }

}