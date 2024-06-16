#pragma once

#include <memory>

namespace nc
{
class Registry;
struct Vector2;

namespace asset
{
class NcAsset;
}

namespace config
{
struct GraphicsSettings;
struct MemorySettings;
struct ProjectSettings;
}

namespace window
{
class NcWindow;
}

namespace graphics
{
struct PerFrameRenderState;
struct PerFrameRenderStateData;
struct ShaderResourceBus;

class IGraphics
{
    public:
        virtual ~IGraphics() = default;

        virtual auto BeginFrame() -> bool = 0;
        virtual void BuildRenderGraph(PerFrameRenderStateData state) = 0;
        virtual void Clear() noexcept = 0;
        virtual auto CurrentFrameIndex() -> uint32_t = 0;
        virtual void DrawFrame(const PerFrameRenderState& state) = 0;
        virtual void FrameEnd() = 0;
        virtual void OnResize(const Vector2& dimensions, bool isMinimized) = 0;
        virtual auto PrepareFrame() -> bool = 0;
};

auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     asset::NcAsset* assetModule,
                     ShaderResourceBus& shaderResourceBus,
                     window::NcWindow& window) -> std::unique_ptr<IGraphics>;
} // namespace graphics
} // namespace nc
