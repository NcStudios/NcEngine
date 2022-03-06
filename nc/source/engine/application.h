#pragma once

#include "MainCamera.h"
#include "graphics/Graphics.h"
#include "ui/UISystemImpl.h"


#include "window/WindowImpl.h"

namespace nc
{
    struct modules;
    namespace graphics { struct PerFrameRenderState; }
    
    
    namespace window { class Window; }

    void attach_application(modules* context, window::WindowImpl* window, MainCamera* mainCamera, bool useModule);

    class application
    {
        public:
            application(window::WindowImpl* window, MainCamera* camera);

            void process_system_messages();
            bool frame_begin();
            void draw(const graphics::PerFrameRenderState& state);
            void frame_end();
            void clear();
            void register_shutdown_callback(std::function<void()> cb);
            auto get_hwnd() -> HWND;
            auto get_graphics() -> graphics::Graphics*;
            auto get_ui() -> ui::UISystemImpl*;
            auto get_window() -> window::WindowImpl*;

        private:
            window::WindowImpl* m_window;
            graphics::Graphics m_graphics;
            ui::UISystemImpl m_ui;
    };
}