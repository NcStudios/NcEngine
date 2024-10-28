#pragma once

#include "ImGuiImplGLFW.h"

#include "ncengine/ui/IUI.h"
#include "ncengine/ui/editor/Editor.h"
#include "ncengine/utility/Signal.h"

namespace nc::graphics
{
// todo: name? ... this really shouldn't be in frontend so...
class UISubsystem
{
    public:
        UISubsystem(Diligent::IRenderDevice& device,
                    const Diligent::SwapChainDesc& swapChainDesc,
                    GLFWwindow* window,
                    ecs::Ecs world,
                    ModuleProvider modules,
                    SystemEvents& events,
                    Signal<>& onFontUpdate)

        auto IsHovered() const noexcept -> bool
        {
            return m_clientUI ? m_clientUI->IsHovered() : false;
        }

        void SetClientUI(ui::IUI* ui) noexcept
        {
            m_clientUI = ui;
        }

        void FrameBegin(Diligent::ISwapChain&)
        {

        }

        void Execute(ecs::Ecs world)
        {
            m_editor->Draw(world);
            if (m_clientUI)
            {
                m_clientUI->Draw();
            }
        }

        void Frame(Diligent::IDeviceContext& context)
        {

        }

    private:
        ImGuiImplGLFW m_imguiBackend;
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
        Connection m_fontConnetion;
};
} // namespace nc::graphics
