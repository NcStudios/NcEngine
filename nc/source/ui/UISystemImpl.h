#pragma once

#include "platform/win32/NCWinDef.h"
#include "ui/UISystem.h"
#include "editor/Editor.h"

namespace nc::graphics 
{ 
    class Graphics;
}

namespace nc::ui
{
    class UISystemImpl final : public UISystem
    {
        public:
            UISystemImpl(HWND hwnd, graphics::Graphics* graphics);

            ~UISystemImpl() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void Set(IUI* ui) noexcept override;
            bool IsHovered() const noexcept override;

            void FrameBegin();

            #ifdef NC_EDITOR_ENABLED
            void Frame(float* dt, Registry* registry);
            #else
            void Frame();
            #endif
            
            void FrameEnd();

        private:
            #ifdef NC_EDITOR_ENABLED
            editor::Editor m_editor;
            #endif
            
            IUI* m_projectUI;
            
            graphics::Graphics* m_graphics;

    };
} // namespace nc::ui