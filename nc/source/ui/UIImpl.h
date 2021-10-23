#pragma once

#include "platform/win32/NCWinDef.h"
#include "ui/IUI.h"
#include "editor/Editor.h"

namespace nc::graphics 
{ 
    class Graphics;
}

namespace nc::ui
{
    class UIImpl
    {
        public:
            UIImpl(HWND hwnd);

            ~UIImpl() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void BindProjectUI(IUI* ui);
            bool IsProjectUIHovered();

            void FrameBegin();

            #ifdef NC_EDITOR_ENABLED
            void Frame(float* dt, registry_type* registry);
            #else
            void Frame();
            #endif
            
            void FrameEnd();

        private:
            #ifdef NC_EDITOR_ENABLED
            editor::Editor m_editor;
            #endif
            
            IUI* m_projectUI;
    };
} // namespace nc::ui