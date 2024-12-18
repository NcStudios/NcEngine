#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/UIBackend.h"
#include "graphics2/frontend/subsystem/UISubsystem.h"

#include "imgui.h"

class UIBackendTest : public DiligentEngineFixture
{
    protected:
        nc::Signal<> onFontUpdate;
        std::unique_ptr<nc::graphics::UIBackend> uiBackend;
        std::unique_ptr<nc::graphics::UISubsystem> uiSubsystem;

        UIBackendTest()
        {
            uiBackend = std::make_unique<nc::graphics::UIBackend>(
                engine->GetDevice(),
                engine->GetSwapChain().GetDesc(),
                window->GetWindowHandle(),
                onFontUpdate
            );
        }

        ~UIBackendTest()
        {
            FailIfHasErrorOutput();
        }
};

TEST_F(UIBackendTest, UIBackend_renderFrame_succeeds)
{
    EXPECT_NO_THROW(uiBackend->FrameBegin(engine->GetSwapChain()));
    EXPECT_NO_THROW(uiBackend->Render(engine->GetContext()));
}

TEST_F(UIBackendTest, UIBackend_onFontUpdate_updatesImguiTextureId)
{
    const auto oldId = ImGui::GetIO().Fonts->TexID;
    onFontUpdate.Emit();
    const auto newId = ImGui::GetIO().Fonts->TexID;
    EXPECT_NE(oldId, newId);
}
