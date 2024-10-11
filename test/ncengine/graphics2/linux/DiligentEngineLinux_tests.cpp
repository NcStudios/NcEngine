#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "../DiligentEngineFixture.inl"
#include "window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace nc::graphics
{
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
