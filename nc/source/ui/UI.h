#pragma once
#include <unordered_map>

#include "NcCommon.h"
#include "win32/NCWinDef.h"
#include "ui/Editor.h"
#include "project/ui/HUD.h"

namespace nc
{
    namespace graphics { class Graphics; }

    namespace ui
    {
        class UI
        {
            public:
                UI(HWND hwnd, nc::graphics::Graphics * graphics);
                ~UI() noexcept;

                LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

                void FrameBegin();
                void Frame(float* dt, float frameLogicTime, std::unordered_map<nc::EntityHandle, nc::Entity>& activeEntities);
                void FrameEnd();

            private:
                #ifdef NC_EDITOR_ENABLED
                editor::Editor m_editor;
                #endif
                
                project::ui::HUD m_hud;
        };
    } //end namespace ui
} //end namespace nc