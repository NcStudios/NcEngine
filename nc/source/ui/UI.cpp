#include "UI.h"
#include "UIStyle.h"
#include "graphics/Graphics.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace nc::ui
{
    UI::UI(HWND hwnd, ::nc::graphics::Graphics * graphics)
        : 
            #ifdef NC_EDITOR_ENABLED
            m_editor{ graphics },
            #endif
            m_hud{}

    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(graphics->m_device.Get(), graphics->m_context.Get());
        SetImGuiStyle();
    }

    UI::~UI()
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }

    LRESULT UI::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
    }

    void UI::BeginFrame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void UI::Frame(float* dt, float frameLogicTime, std::unordered_map<::nc::EntityHandle, ::nc::Entity>& activeEntities)
    {
        m_editor.Frame(dt, frameLogicTime, activeEntities);
        m_hud.Draw();
    }

    void UI::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
} //end namespace nc::ui