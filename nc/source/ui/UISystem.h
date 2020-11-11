#pragma once

#include "win32/NCWinDef.h"
#include "IUI.h"
#include "Editor.h"
#include "ecs/Entity.h"

#include <unordered_map>

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

                static void BindProjectUI(IUI* ui);
                static bool IsProjectUIHovered();

                void FrameBegin();

                #ifdef NC_EDITOR_ENABLED
                void Frame(float* dt, float frameLogicTime, std::unordered_map<nc::EntityHandle, nc::Entity>& activeEntities);
                #else
                void Frame();
                #endif
                
                void FrameEnd();

            private:
                static UISystem* m_instance;

                #ifdef NC_EDITOR_ENABLED
                Editor m_editor;
                #endif
                
                IUI* m_projectUI;
        };
    } //end namespace ui
} //end namespace nc