#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "../DiligentEngineIntFixture.inl"
#include "../../../../ncengine/graphics2/DiligentEngineFixture.inl"
#include "window/Window.h"

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
} // namespace nc::graphics
