#include "WireframeTechnique.h"
#include "graphics/TechniqueType.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ShaderResources.h"
#include "graphics/d3dresource/MeshResources.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "config/Config.h"

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

        /** @note A step is required w/ the curring setup, even if it does no work. 
         *  I'm leaving it for now as it isn't clear how vulkan will change this. */
        Step single(1);
        {
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

        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Rasterizer>(Rasterizer::GetUID(Rasterizer::Mode::Wireframe), Rasterizer::Mode::Wireframe));
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Stencil>(Stencil::GetUID(Stencil::Mode::Off), Stencil::Mode::Off));

        // Add vertex shader
        const auto defaultShaderPath = nc::config::Get().graphics.d3dShadersPath;
        const auto vertexShaderPath = defaultShaderPath + "wireframevertexshader.cso";
        auto pvs = GraphicsResourceManager::AcquireOnDemand<VertexShader>(VertexShader::GetUID(vertexShaderPath), vertexShaderPath);
        auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
        WireframeTechnique::m_commonResources.push_back(std::move(pvs));
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<InputLayout>(InputLayout::GetUID("wireframevertexshader"), WireframeInputElementDesc, pvsbc));

        // Add pixel shader
        const auto pixelShaderPath = defaultShaderPath + "wireframepixelshader.cso";
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<PixelShader>(PixelShader::GetUID(pixelShaderPath), pixelShaderPath));
        WireframeTechnique::m_commonResources.push_back(GraphicsResourceManager::AcquireOnDemand<Blender>(Blender::GetUID(BLENDER_TAG), false));
    }

    void WireframeTechnique::BindCommonResources()
    {
        for(const auto& res : WireframeTechnique::m_commonResources)
        {
            res->Bind();
        }
    }
}