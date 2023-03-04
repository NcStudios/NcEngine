#pragma once

#include "platform/win32/NcWin32.h"

#include <memory>

namespace nc
{
struct GpuAccessorSignals;
class Registry;

namespace config
{
struct GraphicsSettings;
struct ProjectSettings;
}

namespace window
{
class WindowImpl;
}

namespace graphics
{
struct PerFrameRenderState;

class IGraphics
{
    public:
        virtual ~IGraphics() = default;

        virtual void InitializeUI() = 0;
        virtual auto FrameBegin() -> bool = 0;
        virtual void Draw(const PerFrameRenderState& state) = 0;
        virtual void FrameEnd() = 0;
        virtual void OnResize(float width, float height, const WPARAM windowArg) = 0;
        virtual void Clear() noexcept = 0;
};

auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     const GpuAccessorSignals& signals,
                     Registry* registry,
                     window::WindowImpl* window) -> std::unique_ptr<IGraphics>;
} // namespace graphics
} // namespace nc
