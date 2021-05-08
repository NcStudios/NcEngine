#pragma once

#include "platform/win32/NCWinDef.h"
#include "ui/IUI.h"
#include "editor/Editor.h"
#include "entity/Entity.h"
#include "ecs/EntityComponentSystem.h"

namespace nc::graphics 
{ 
    #ifdef USE_VULKAN
    class Graphics2;
    #endif
    class Graphics;
}

namespace nc::ui
{
    class UIImpl
    {
        public:
            #ifdef NC_EDITOR_ENABLED
                #ifdef USE_VULKAN
                UIImpl(HWND hwnd, graphics::Graphics2* graphics, ecs::RendererSystem* rendererSystem, const ecs::Systems& systems);
                #else
                UIImpl(HWND hwnd, graphics::Graphics* graphics, const ecs::Systems& systems);
                #endif
            #else
                #ifdef USE_VULKAN
                UIImpl(HWND hwnd, graphics::Graphics2* graphics, ecs::RendererSystem* rendererSystem);
                #else
                UIImpl(HWND hwnd, graphics::Graphics* graphics);
                #endif
            #endif

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
            
            #ifdef USE_VULKAN
            graphics::Graphics2* m_graphics;
            ecs::RendererSystem* m_rendererSystem;
            #endif

    };
} // namespace nc::ui