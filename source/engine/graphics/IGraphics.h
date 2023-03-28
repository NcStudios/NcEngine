#pragma once

#include <memory>

namespace nc
{
class Registry;

namespace asset
{
class NcAsset;
}

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
struct ShaderResourceBus;

class IGraphics
{
    public:
        virtual ~IGraphics() = default;

        virtual void InitializeUI() = 0;
        virtual auto FrameBegin() -> bool = 0;
        virtual void Draw(const PerFrameRenderState& state) = 0;
        virtual void FrameEnd() = 0;
        virtual void OnResize(float width, float height, bool isMinimized) = 0;
        virtual void Clear() noexcept = 0;
};

auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     asset::NcAsset* assetModule,
                     ShaderResourceBus& shaderResourceBus,
                     Registry* registry,
                     window::WindowImpl* window) -> std::unique_ptr<IGraphics>;
} // namespace graphics
} // namespace nc
