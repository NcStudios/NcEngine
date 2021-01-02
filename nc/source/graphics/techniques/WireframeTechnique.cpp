#include "WireframeTechnique.h"
#include "TechniqueType.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ShaderResources.h"
#include "graphics/d3dresource/MeshResources.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "Engine.h"

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
    WireframeTechnique::WireframeTechnique()
    {
        auto defaultShaderPath = nc::engine::Engine::GetConfig().graphics.shadersPath;

        Step single(1);
        {
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<Stencil>(Stencil::Mode::Off));
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<Rasterizer>(Rasterizer::Mode::Wireframe));

            // Add vertex shader
            auto pvs = GraphicsResourceManager::Acquire<VertexShader>(defaultShaderPath + "wireframevertexshader.cso");
            auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
            single.AddGraphicsResource(std::move(pvs));
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<InputLayout>("wireframevertexshader", WireframeInputElementDesc, pvsbc));

            // Add pixel shader
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::PixelShader>(defaultShaderPath + "wireframepixelshader.cso"));
            single.AddGraphicsResource(GraphicsResourceManager::Acquire<d3dresource::Blender>(BLENDER_TAG));
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
    }
    
    #endif

}