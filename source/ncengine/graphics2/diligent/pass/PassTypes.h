#pragma once

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

#include <limits>
#include <span>
#include <string_view>
#include <vector>

namespace nc::graphics
{
constexpr auto NoTarget = std::numeric_limits<uint32_t>::max()-1;
constexpr auto SwapChainColorRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto SwapChainDepthRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;

/* MSAA Color Target Indices */
constexpr auto MainColorMsaa = 0u;
constexpr auto NormalsColorMsaa = 1u;

/* Color Target Indices */
constexpr auto MainColor = 0u;
constexpr auto NormalsColor = 1u;
constexpr auto PPWaveColor = 2u;
constexpr auto PPOutlineColor = 3u;

/* MSAA Depth Target Indices */
constexpr auto MainDepthMsaa = 0u;
constexpr auto NormalsDepthMsaa = 1u;

/* Depth Target Indices */
constexpr auto MainDepth = 0u;

struct ShaderPaths
{
    std::string_view pixelShaderPath = "Uninitialized";
    std::string_view vertexShaderPath = "Uninitialized";
};

enum class PassType : uint8_t
{
    None            = 0,
    Material        = 1,
    SkinnedMaterial = 1 << 1,
    Wireframe       = 1 << 2,
    PostProcess     = 1 << 3
};

struct PassDesc
{
    uint64_t id = 0;
    std::string_view name  = "Uninitialized";
    PassType type = PassType::None;
    ShaderPaths shaderPaths = ShaderPaths{};
    std::vector<uint32_t> colorSources = std::vector<uint32_t>{};
    std::vector<uint32_t> depthSources = std::vector<uint32_t>{};
    uint32_t colorSink = NoTarget;
    uint32_t depthSink = NoTarget;
    bool isMsaa = true;
};
} // namespace nc::graphics
