#pragma once

#include "ImGuiImplGLFW.h"

#include "ncengine/ui/IUI.h"
#include "ncengine/ui/editor/Editor.h"
#include "ncengine/utility/Signal.h"

#include "Graphics/GraphicsEngine/interface/SwapChain.h"

namespace nc::graphics
{
// todo: name? ... this really shouldn't be in frontend so...
class UISubsystem
{
    public:
        UISubsystem(Diligent::IRenderDevice& device,
                    const Diligent::SwapChainDesc& swapChainDesc,
                    GLFWwindow* window,
                    std::string_view api,
                    ecs::Ecs world,
                    ModuleProvider modules,
                    SystemEvents& events,
                    Signal<>& onFontUpdate)
            : m_imguiBackend{device, swapChainDesc, window, api},
              m_editor{ui::editor::BuildEditor(world, modules, events)},
              m_fontConnetion{onFontUpdate.Connect(this, &UISubsystem::OnFontUpdate)}
        {
        }

        auto IsHovered() const noexcept -> bool
        {
            return m_clientUI ? m_clientUI->IsHovered() : false;
        }

        void SetClientUI(ui::IUI* ui) noexcept
        {
            m_clientUI = ui;
        }

        void FrameBegin(Diligent::ISwapChain& swapChain)
        {
            const auto& scDesc = swapChain.GetDesc();
            m_imguiBackend.NewFrame(scDesc.Width, scDesc.Height, scDesc.PreTransform);
        }

        void UpdateUI(ecs::Ecs world)
        {
            m_editor->Draw(world);
            if (m_clientUI)
            {
                m_clientUI->Draw();
            }
        }

        void Render(Diligent::IDeviceContext& context)
        {
            m_imguiBackend.Render(&context);
        }

    private:
        ImGuiImplGLFW m_imguiBackend;
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
        Connection m_fontConnetion;

        void OnFontUpdate()
        {
            m_imguiBackend.UpdateFontsTexture();
        }
};
} // namespace nc::graphics
