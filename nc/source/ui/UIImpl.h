#pragma once

#include "platform/win32/NCWinDef.h"
#include "ui/IUI.h"
#include "editor/Editor.h"
#include "entity/Entity.h"
#include "ecs/EntityMap.h"

#include <unordered_map>

namespace nc
{
    namespace graphics { class Graphics; }

    namespace ui
    {
        class UIImpl
        {
            public:
                UIImpl(HWND hwnd, nc::graphics::Graphics* graphics);
                ~UIImpl() noexcept;

                LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

                void BindProjectUI(IUI* ui);
                bool IsProjectUIHovered();

                void FrameBegin();

                #ifdef NC_EDITOR_ENABLED
                void Frame(float* dt, ecs::EntityMap& activeEntities);
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
    } //end namespace ui
} //end namespace nc