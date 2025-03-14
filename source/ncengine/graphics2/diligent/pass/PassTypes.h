#pragma once

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

#include "ncengine/utility/EnumUtilities.h"

#include <limits>
#include <span>
#include <string_view>
#include <vector>

namespace nc::graphics
{
constexpr auto DepthStencilTarget = std::numeric_limits<uint32_t>::max();
constexpr auto SwapChainTarget = std::numeric_limits<uint32_t>::max();
constexpr auto NoTarget = std::numeric_limits<uint32_t>::max() - 1;

constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;
constexpr auto OffScreenShadowMapRTFormat = Diligent::TEX_FORMAT_R32_FLOAT;

enum class ColorTarget : uint8_t
{
    None,
    Swapchain,
    Main,
    Normals
};

enum class DepthTarget : uint8_t
{
    None,
    DepthStencil,
    Main
};

enum class PostProcessTarget : uint8_t
{
    None,
    PPOutline,
    PPFxaa,
    PPNoise
};

enum class ShadowMapTarget : uint8_t
{
    None,
    Uni,
    Point
};

struct Sources
{
    std::vector<uint32_t> color = std::vector<uint32_t>();
    std::vector<uint32_t> depth = std::vector<uint32_t>();
    uint32_t postProcess = 0u;
};

struct Sinks
{
    uint32_t color = NoTarget;
    uint32_t depth = NoTarget;
    uint32_t postProcess = NoTarget;
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
    uint64_t flag = 0;
    std::string_view name  = "";
    PassType type = PassType::None;
    ShaderPaths shaderPaths = ShaderPaths{};
    std::vector<ColorTarget> colorSources = std::vector<ColorTarget>{};
    std::vector<DepthTarget> depthSources = std::vector<DepthTarget>{};
    PostProcessTarget postProcessSource = PostProcessTarget::None;
    ColorTarget colorSink = ColorTarget::None;
    DepthTarget depthSink = DepthTarget::None;
    ShadowMapTarget shadowMapSink = ShadowMapTarget::None;
    PostProcessTarget postProcessSink = PostProcessTarget::None;
    bool isMsaa = true;
    bool useDepthTest = true;
};
} // namespace nc::graphics
