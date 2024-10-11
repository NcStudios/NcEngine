#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "../DiligentEngineIntFixture.inl"
#include "window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace nc::graphics
{
TEST(DiligentEngineWin32_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_NO_THROW(CreateDiligentEngine(false, "vulkan", supportedApis, engineCI));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D11LNotHeadless_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"d3d11"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_NO_THROW(CreateDiligentEngine(false, "d3d11", supportedApis, engineCI));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12LNotHeadless_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"d3d12"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_NO_THROW(CreateDiligentEngine(false, "d3d12", supportedApis, engineCI));
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_D3D12RenderTriangle_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"d3d12"};
    auto engineCI = Diligent::EngineCreateInfo{};

    /* Create window */
    auto info = nc::window::WindowInfo{.isHeadless = false};
    auto ncWindow = nc::window::NcWindowImpl{info};
    auto engine = CreateDiligentEngine(false, "d3d12", supportedApis, engineCI, &ncWindow);

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
    auto supportedApis = std::vector<std::string_view>{"d3d11"};
    auto engineCI = Diligent::EngineCreateInfo{};

    /* Create window */
    auto info = nc::window::WindowInfo{.isHeadless = false};
    auto ncWindow = nc::window::NcWindowImpl{info};
    auto engine = CreateDiligentEngine(false, "d3d11", supportedApis, engineCI, &ncWindow);

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
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};

    /* Create window */
    auto info = nc::window::WindowInfo{.isHeadless = false};
    auto ncWindow = nc::window::NcWindowImpl{info};
    auto engine = CreateDiligentEngine(false, "vulkan", supportedApis, engineCI, &ncWindow);

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

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_Headless_Succeeds)
{
    auto supportedApis = nc::graphics::GetSupportedApis();
    auto engineCI = Diligent::EngineCreateInfo{};

    for (const auto& api : supportedApis)
    {
        EXPECT_NO_THROW(CreateDiligentEngine(true, api, supportedApis, engineCI));
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    auto supportedApis = nc::graphics::GetSupportedApis();

    if (!supportedApis.empty())
    {
        auto engineCI = Diligent::EngineCreateInfo{};
        EXPECT_THROW(CreateDiligentEngine(true, "notInList", supportedApis, engineCI), nc::NcError);
    }
}

TEST(DiligentEngineWin32_tests, CreateDiligentEngine_TargetAPIFails_FallbackIsAttemptedAndFails)
{
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    engineCI.EngineAPIVersion = 500;

    /* This test only applies if more than one API exists in supported APIs list*/
    if (supportedApis.size() < 2)
    {
        return;
    }

    for (const auto& api : supportedApis)
    {
        try
        {
            CreateDiligentEngine(true, api, supportedApis, engineCI);
        }
        catch (const std::runtime_error& error)
        {
            auto errorStr = std::string_view(error.what());
            auto otherApis = ExcludeElementFromContainer(supportedApis, api);

            for (const auto& otherApi : otherApis)
            {
                ASSERT_EQ(errorStr.contains(fmt::format("Failed to create the {0} device or context", GetFullApiName(otherApi))), true);
            }
            ASSERT_EQ(errorStr.contains("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize."), true);
        }
    }
}
} // namespace nc::graphics
