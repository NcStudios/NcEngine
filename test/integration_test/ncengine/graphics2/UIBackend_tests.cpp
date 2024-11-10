#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/UIBackend.h"
#include "graphics2/frontend/subsystem/UISubsystem.h"

#include "imgui.h"

class UIBackendTest : public DiligentEngineParameterizedFixture
{
    protected:
        nc::Signal<> onFontUpdate;
        std::unique_ptr<nc::graphics::UIBackend> uiBackend;
        std::unique_ptr<nc::graphics::UISubsystem> uiSubsystem;

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;

            uiBackend = std::make_unique<nc::graphics::UIBackend>(
                engine->GetDevice(),
                engine->GetSwapChain().GetDesc(),
                window->GetWindowHandle(),
                GetTestApi(),
                onFontUpdate
            );
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, UIBackendTest, g_apiParams);

TEST_P(UIBackendTest, UIBackend_renderFrame_succeeds)
{
    EXPECT_NO_THROW(uiBackend->FrameBegin(engine->GetSwapChain()));
    EXPECT_NO_THROW(uiBackend->Render(engine->GetContext()));
}

TEST_P(UIBackendTest, UIBackend_onFontUpdate_updatesImguiTextureId)
{
    const auto oldId = ImGui::GetIO().Fonts->TexID;
    onFontUpdate.Emit();
    const auto newId = ImGui::GetIO().Fonts->TexID;
    EXPECT_NE(oldId, newId);
}
