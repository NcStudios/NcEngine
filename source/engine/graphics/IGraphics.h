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
struct MemorySettings;
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

        virtual void CommitResourceLayout() = 0;
        virtual auto PrepareFrame() -> bool = 0;
        virtual auto BeginFrame() -> bool = 0;
        virtual auto CurrentFrameIndex() -> uint32_t = 0;
        virtual void DrawFrame(const PerFrameRenderState& state) = 0;
        virtual void FrameEnd() = 0;
        virtual void OnResize(float width, float height, bool isMinimized) = 0;
        virtual void Clear() noexcept = 0;
};

auto GraphicsFactory(const config::ProjectSettings& projectSettings,
                     const config::GraphicsSettings& graphicsSettings,
                     const config::MemorySettings& memorySettings,
                     asset::NcAsset* assetModule,
                     ShaderResourceBus& shaderResourceBus,
                     Registry* registry,
                     window::WindowImpl* window) -> std::unique_ptr<IGraphics>;
} // namespace graphics
} // namespace nc
