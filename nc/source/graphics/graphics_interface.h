#pragma once

#include "platform/win32/NcWin32.h"

#include <array>

namespace nc::graphics
{
    struct PerFrameRenderState;

    class graphics_interface
    {
        public:
            ~graphics_interface() = default;

            virtual void initialize_ui() = 0;
            virtual bool frame_begin() = 0;
            virtual void draw(const PerFrameRenderState&) = 0;
            virtual void frame_end() = 0;
            virtual void clear() = 0;
            virtual void on_resize(float width, float height, float nearZ, float farZ, WPARAM windowArg) = 0;
            virtual void set_clear_color(std::array<float, 4> color) = 0;
    };
}