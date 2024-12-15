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
constexpr auto MainColor = 0u;
constexpr auto MainDepth = 0u;
constexpr auto NormalsColor = 1u;
constexpr auto PPWaveColor = 2u;
constexpr auto PPOutlineColor = 3u;

struct ShaderPaths
{
    std::string_view pixelShaderPath = "Uninitialized";
    std::string_view vertexShaderPath = "Uninitialized";
};

enum class PassType : uint8_t
{
    None,
    Material,
    Wireframe,
    PostProcess,
    UI
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
};
} // namespace nc::graphics
