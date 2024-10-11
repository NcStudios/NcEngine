#include "gtest/gtest.h"
#include "config/Config.h"
#include "graphics2/diligent/DiligentEngine.h"
#include "graphics/NcGraphics.h"
#include "../DiligentEngineFixture.inl"
#include "window/Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

using namespace nc::graphics;

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_VulkanHeadless_Succeeds)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_NO_THROW(CreateDiligentEngine(true, "vulkan", supportedApis, engineCI));
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_APINotInSupportedApisList_Fails)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};
    auto engineCI = Diligent::EngineCreateInfo{};
    EXPECT_THROW(CreateDiligentEngine(true, "notInList", supportedApis, engineCI), nc::NcError);
}

TEST(DiligentEngineLinux_tests, CreateDiligentEngine_TargetAPIFails_NoFallback_Fails)
{
    auto supportedApis = std::vector<std::string_view>{"vulkan"};

    /* Create DiligentEngine */
    Diligent::EngineCreateInfo engineCI{};
    engineCI.EngineAPIVersion = 500;
    try 
    {
        CreateDiligentEngine(true, "vulkan", supportedApis, engineCI);
    }
    catch (const std::runtime_error& error)
    {
        auto errorStr = std::string_view(error.what());
        ASSERT_EQ(errorStr.contains("Failed to create the Vulkan device or context"), true);
        ASSERT_EQ(errorStr.contains("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize."), true);
    }
}