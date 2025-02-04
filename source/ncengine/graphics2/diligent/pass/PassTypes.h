#pragma once

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

#include "ncengine/utility/EnumUtilities.h"

#include <limits>
#include <span>
#include <string_view>
#include <vector>

namespace nc::graphics
{
constexpr auto SwapChainIndex = std::numeric_limits<uint32_t>::max();
constexpr auto NoneIndex = std::numeric_limits<uint32_t>::max() - 1;

constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;

enum class ColorBuffer : uint8_t
{
    None,
    Swapchain,
    Main,
    Normals,
};

enum class DepthBuffer : uint8_t
{
    None,
    DepthStencil,
    Main
};

enum class PostProcessBuffer : uint8_t
{
    None,
    PPOutline,
    PPFxaa
};

struct Sources
{
    std::vector<uint32_t> color = std::vector<uint32_t>();
    std::vector<uint32_t> depth = std::vector<uint32_t>();
    uint32_t postProcess = 0u;
};

struct Sinks
{
    uint32_t color;
    uint32_t depth;
    uint32_t postProcess;
};

struct ShaderPaths
{
    std::string_view pixelShaderPath = "";
    std::string_view vertexShaderPath = "";
};

enum class PassType : uint8_t
{
    None            = 0,
    Material        = 1,
    SkinnedMaterial = 1 << 1,
    Wireframe       = 1 << 2,
    Particle        = 1 << 3,
    PostProcess     = 1 << 4
}; DEFINE_BITWISE_OPERATORS(PassType);

struct PassDesc
{
    uint64_t id = 0;
    std::string_view name  = "";
    PassType type = PassType::None;
    ShaderPaths shaderPaths = ShaderPaths{};
    std::vector<ColorBuffer> colorSources = std::vector<ColorBuffer>{};
    std::vector<DepthBuffer> depthSources = std::vector<DepthBuffer>{};
    PostProcessBuffer postProcessSource = PostProcessBuffer::None;
    ColorBuffer colorSink = ColorBuffer::None;
    DepthBuffer depthSink = DepthBuffer::None;
    PostProcessBuffer postProcessSink = PostProcessBuffer::None;
    bool isMsaa = true;
};
} // namespace nc::graphics
