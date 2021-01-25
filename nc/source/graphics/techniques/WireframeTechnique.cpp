#include "WireframeTechnique.h"
#include "TechniqueType.h"
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
    const auto WireframeInputElementDesc = std::vector<D3D11_INPUT_ELEMENT_DESC>
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    const std::string BLENDER_TAG = "texture_blender_resource";
}

namespace nc::graphics
{
    std::vector<d3dresource::GraphicsResource*> WireframeTechnique::m_commonResources = {};

    WireframeTechnique::WireframeTechnique()
    {
        WireframeTechnique::InitializeCommonResources();

        Step single(1);
        {
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<Rasterizer>(Rasterizer::Mode::Wireframe));
        }
        AddStep(std::move(single));
    }

    size_t WireframeTechnique::GetUID() noexcept
    {
        const size_t name = 100;
        return name;
    }

    #ifdef NC_EDITOR_ENABLED
    void WireframeTechnique::EditorGuiElement()
    {
        ImGui::SameLine();
        ImGui::Text("(Wireframe)");
    }
    #endif

    void WireframeTechnique::InitializeCommonResources()
    {
        static bool isInitialized = false;
        if(isInitialized)
        {
            return;
        }

        isInitialized = true;

        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::Acquire<Stencil>(Stencil::Mode::Off));

        // Add vertex shader
        auto defaultShaderPath = nc::engine::Engine::GetConfig().graphics.shadersPath;
        auto pvs = GraphicsResourceManager::Acquire<VertexShader>(defaultShaderPath + "wireframevertexshader.cso");
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        WireframeTechnique::m_commonResources.push_back(std::move(pvs));
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::Acquire<InputLayout>("wireframevertexshader", WireframeInputElementDesc, pvsbc));

        // Add pixel shader
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::Acquire<d3dresource::PixelShader>(defaultShaderPath + "wireframepixelshader.cso"));
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::Acquire<d3dresource::Blender>(BLENDER_TAG));
    }

    void WireframeTechnique::BindCommonResources()
    {
        for(const auto& res : WireframeTechnique::m_commonResources)
        {
            res->Bind();
        }
    }
}