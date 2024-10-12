#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "window/Window.h"
#include "../../../ncengine/window/NcWindowStub.inl"

static const char* VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn) 
{
    float4 Pos[4];
    Pos[0] = float4(-1.0, -1.0, 0.0, 1.0);  // Bottom-left corner
    Pos[1] = float4(-1.0,  1.0, 0.0, 1.0);  // Top-left corner
    Pos[2] = float4( 1.0,  1.0, 0.0, 1.0);  // Top-right corner
    Pos[3] = float4( 1.0, -1.0, 0.0, 1.0);  // Bottom-right corner

    float3 Color = float3(1.0, 0.0, 0.0); // red

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Color;
}
)";

// Pixel shader simply outputs interpolated vertex color
static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";

void SetupSquare(nc::graphics::DiligentEngine* engine, Diligent::IPipelineState** ppPso)
{
    using namespace Diligent;

    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Simple square PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = engine->SwapChain()->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = engine->SwapChain()->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Square vertex shader";
        ShaderCI.Source          = VSSource;
        engine->Device()->CreateShader(ShaderCI, &pVS);
    }

    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Square pixel shader";
        ShaderCI.Source          = PSSource;
        engine->Device()->CreateShader(ShaderCI, &pPS);
    }

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    engine->Device()->CreateGraphicsPipelineState(PSOCreateInfo, ppPso);
}

void RenderSquare(nc::graphics::DiligentEngine* engine, Diligent::IPipelineState* pPso)
{
    using namespace Diligent;

    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    auto* pRTV = engine->SwapChain()->GetCurrentBackBufferRTV();
    auto* pDSV = engine->SwapChain()->GetDepthBufferDSV();
    engine->Context()->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    engine->Context()->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    engine->Context()->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    engine->Context()->SetPipelineState(pPso);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 4;
    engine->Context()->Draw(drawAttrs);
}

auto CreateDiligentEngine(bool isHeadless, std::string_view targetApi, std::span<const std::string_view> supportedApis, Diligent::EngineCreateInfo engineCI, nc::window::NcWindowStub* window = nullptr) -> nc::graphics::DiligentEngine
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = isHeadless;
    graphicsSettings.api = targetApi;
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineLinux_tests";

    if (!window)
    {
        /* Create window */
        auto info = nc::window::WindowInfo{.isHeadless = graphicsSettings.isHeadless};
        auto ncWindow = nc::window::NcWindowStub{info};

        /* Create DiligentEngine */
        return nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);
    }

    /* Create DiligentEngine */
     return nc::graphics::DiligentEngine(graphicsSettings, engineCI, window->GetWindowHandle(), supportedApis);
}

auto GetFullApiName(const std::string_view api) -> std::string_view
{
    if (api == "vulkan")
    {
        return "Vulkan";
    }
    if (api == "d3d12")
    {
        return "D3D12";
    }
    if (api == "d3d11")
    {
        return "D3D11";
    }

    throw std::runtime_error("API not in list.");
}

std::vector<std::string_view> ExcludeElementFromContainer(std::span<const std::string_view> inputSpan, const std::string_view& elementToExclude)
{
    std::vector<std::string_view> result;
    result.reserve(inputSpan.size());

    for (const auto& elem : inputSpan)
    {
        if (elem != elementToExclude)
        {
            result.push_back(elem);
        }
    }

    return result;
}
