#pragma once

#include "platform/win32/NCWin32.h"
#include "editor/Editor.h"
#include "ui/IUI.h"

namespace nc::ui
{
    class UISystemImpl final
    {
        public:
            UISystemImpl(HWND hwnd);
            ~UISystemImpl() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void Set(IUI* ui) noexcept;
            bool IsHovered() const noexcept;

            void FrameBegin();

            #ifdef NC_EDITOR_ENABLED
            void Draw(float* dt, Registry* registry);
            #else
            void Draw();
            #endif
            
        private:
            #ifdef NC_EDITOR_ENABLED
            editor::Editor m_editor;
            #endif

            IUI* m_projectUI;
    };
} // namespace nc::ui