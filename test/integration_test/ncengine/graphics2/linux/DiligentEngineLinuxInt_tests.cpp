#include "gtest/gtest.h"
#include "../DiligentEngineIntFixture.inl"
#include "graphics2/diligent/DiligentEngine.h"
#include "ncengine/config/Config.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace nc::graphics
{
TEST(DiligentEngineLinux_tests, CreateDiligentEngine_VulkanNotHeadless_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_NO_THROW(CreateDiligentEngine(false, "vulkan", supportedApis, engineCI));
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_VulkanRenderTriangle_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};

    /* Create window */
    auto info = nc::window::WindowInfo{.isHeadless = false};
    auto ncWindow = nc::window::NcWindowStub{info};
    auto engine = CreateDiligentEngine(false, "vulkan", supportedApis, engineCI, &ncWindow);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
    SetupSquare(&engine, m_pPSO.RawDblPtr());

    auto frameCountMax = 60u;
    auto currentFrameIndex = 0u;
    while (currentFrameIndex < frameCountMax)
    {
        ncWindow.ProcessSystemMessages();
        RenderSquare(&engine, m_pPSO.RawPtr());
        engine.GetSwapChain().Present();
        currentFrameIndex++;
    }
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_Headless_Succeeds)
{
    auto supportedApis = nc::graphics::GetSupportedApis();
    auto engineCI = Diligent::EngineCreateInfo{};

    for (const auto& api : supportedApis)
    {
        EXPECT_NO_THROW(CreateDiligentEngine(true, api, supportedApis, engineCI));
    }
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    auto supportedApis = nc::graphics::GetSupportedApis();

    if (!supportedApis.empty())
    {
        auto engineCI = Diligent::EngineCreateInfo{};
        EXPECT_THROW(CreateDiligentEngine(true, "notInList", supportedApis, engineCI), nc::NcError);
    }
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_TargetAPIFails_NoFallback_Fails)
{
    auto supportedApis = nc::graphics::GetSupportedApis();
    Diligent::EngineCreateInfo engineCI{};
    engineCI.EngineAPIVersion = 500;

    for (const auto& api : supportedApis)
    {
        EXPECT_THROW(CreateDiligentEngine(true, api, supportedApis, engineCI), nc::NcError);
    }
}
} // namespace nc::graphics
