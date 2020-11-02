#pragma once
#include <unordered_map>

#include "NcCommon.h"
#include "win32/NCWinDef.h"
#include "IUI.h"
#include "Editor.h"

namespace nc
{
    namespace graphics { class Graphics; }

    namespace ui
    {
        class UISystem
        {
            public:
                UISystem(HWND hwnd, nc::graphics::Graphics * graphics);
                ~UISystem() noexcept;

                LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

                void BindProjectUI(IUI* ui);
                bool IsProjectUIHovered() const;

                void FrameBegin();
                void Frame(float* dt, float frameLogicTime, std::unordered_map<nc::EntityHandle, nc::Entity>& activeEntities);
                void FrameEnd();

            private:
                #ifdef NC_EDITOR_ENABLED
                Editor m_editor;
                #endif
                
                IUI* m_projectUI;
        };
    } //end namespace ui
} //end namespace nc