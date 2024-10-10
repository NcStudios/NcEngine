#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace nc::window
{
class NcWindowImpl : public NcWindow
{
    public:
        NcWindowImpl(WindowInfo info)
        : m_window{nullptr}
        {
            if (!glfwInit())
                throw std::runtime_error("Could not init GLFW.");

            SetWindow(info);
        };

        ~NcWindowImpl()
        {
            if (m_window)
            {
                glfwDestroyWindow(m_window);
            }
            glfwTerminate();
        }

        void SetWindow(WindowInfo info) override 
        {
            if (m_window)
                glfwDestroyWindow(m_window);
            
            if (info.isHeadless)
            {
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            }
            else
            {
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            }

            if (!info.isGL)
            {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            }
            else 
            {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            }
            
            m_window = glfwCreateWindow(100, 100, "DiligentEngineWin32_tests", nullptr, nullptr);
            if (!m_window)
            {
                throw std::runtime_error("Could not create window.");
            }
        };

        void DestroyWindow()
        {
            if (m_window)
                glfwDestroyWindow(m_window);
        }

        void ProcessSystemMessages() override
        {
            glfwPollEvents();
        };

        auto GetWindowHandle() const noexcept -> GLFWwindow* 
        { 
            return m_window;
        }

    private:
        GLFWwindow* m_window;

};
} // namespace nc::window

namespace
{
void ExpectDiligentInitSucceeds(bool isHeadless, std::string targetApi, bool isGLWindowContext)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = isHeadless;
    graphicsSettings.api = targetApi;
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = isGLWindowContext, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    EXPECT_NO_THROW(nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis));
}

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
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
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
        ShaderCI.Desc.Name       = "Triangle vertex shader";
        ShaderCI.Source          = VSSource;
        engine->Device()->CreateShader(ShaderCI, &pVS);
    }

    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Triangle pixel shader";
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
} // anonymous namespace

using namespace nc::graphics;

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "vulkan", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "vulkan", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "opengl", true);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "opengl", true);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11LNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "d3d11", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11Headless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "d3d11", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12LNotHeadless_Succeeds)
{
    ExpectDiligentInitSucceeds(false, "d3d12", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12Headless_Succeeds)
{
    ExpectDiligentInitSucceeds(true, "d3d12", false);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "notInList";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    std::vector<std::string_view> supportedApis =
    {
        "vulkan"
    };

    /* Create DiligentEngine */
    Diligent::EngineCreateInfo engineCI{};
    EXPECT_THROW(DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis), nc::NcError);
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_TargetAPIFails_FallbackIsAttemptedAndFails)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "vulkan";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    std::vector<std::string_view> supportedApis =
    {
        "vulkan",
        "d3d12"
    };

    /* Create DiligentEngine */
    Diligent::EngineCreateInfo engineCI{};
    engineCI.EngineAPIVersion = 500;
    try 
    {
        DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);
    }
    catch (const std::runtime_error& error)
    {
        auto errorStr = std::string_view(error.what());
        ASSERT_EQ(errorStr.contains("Failed to create the Vulkan device or context"), true);
        ASSERT_EQ(errorStr.contains("Failed to create the D3D12 device or context."), true);
        ASSERT_EQ(errorStr.contains("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize."), true);
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12RenderTriangle_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "d3d12";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    auto engine = nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
    SetupSquare(&engine, m_pPSO.RawDblPtr());

    auto frameCountMax = 60u;
    auto currentFrameIndex = 0u;
    while (currentFrameIndex < frameCountMax)
    {
        ncWindow.ProcessSystemMessages();
        RenderSquare(&engine, m_pPSO.RawPtr());
        engine.SwapChain()->Present();
        currentFrameIndex++;
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11RenderTriangle_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "d3d11";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    auto engine = nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
    SetupSquare(&engine, m_pPSO.RawDblPtr());

    auto frameCountMax = 60u;
    auto currentFrameIndex = 0u;
    while (currentFrameIndex < frameCountMax)
    {
        ncWindow.ProcessSystemMessages();
        RenderSquare(&engine, m_pPSO.RawPtr());
        engine.SwapChain()->Present();
        currentFrameIndex++;
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_OpenGLRenderTriangle_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "opengl";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = true, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    auto engine = nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
    SetupSquare(&engine, m_pPSO.RawDblPtr());

    auto frameCountMax = 60u;
    auto currentFrameIndex = 0u;
    while (currentFrameIndex < frameCountMax)
    {
        ncWindow.ProcessSystemMessages();
        RenderSquare(&engine, m_pPSO.RawPtr());
        engine.SwapChain()->Present();
        currentFrameIndex++;
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanRenderTriangle_Succeeds)
{
    /* Create config */
    auto graphicsSettings = nc::config::GraphicsSettings();
    graphicsSettings.isHeadless = false;
    graphicsSettings.api = "vulkan";
    auto projectSettings = nc::config::ProjectSettings();
    projectSettings.projectName = "DiligentEngineWin32_tests";

    /* Create window */
    auto info = nc::window::WindowInfo{ .isGL = false, .isHeadless = graphicsSettings.isHeadless};
    auto ncWindow = nc::window::NcWindowImpl{info};

    /* Create DiligentEngine */
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    auto engine = nc::graphics::DiligentEngine(graphicsSettings, engineCI, ncWindow.GetWindowHandle(), supportedApis);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
    SetupSquare(&engine, m_pPSO.RawDblPtr());

    auto frameCountMax = 60u;
    auto currentFrameIndex = 0u;
    while (currentFrameIndex < frameCountMax)
    {
        ncWindow.ProcessSystemMessages();
        RenderSquare(&engine, m_pPSO.RawPtr());
        engine.SwapChain()->Present();
        currentFrameIndex++;
    }
}
