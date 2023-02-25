#pragma once

#include "platform/win32/NcWin32.h"

namespace nc::graphics
{
class PerFrameRenderState;

class IGraphics
{
    public:
        virtual ~IGraphics() = default;

        virtual void Initialize() = 0;
        virtual void Clear() noexcept = 0;
        virtual bool FrameBegin() = 0;
        virtual void Draw(const PerFrameRenderState& state) = 0;
        virtual void FrameEnd() = 0;
        virtual void OnResize(float width, float height, WPARAM windowArg) = 0;
};
} // namespace nc::graphics
