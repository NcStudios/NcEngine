#pragma once

#include <array>
#include <limits>
#include <span>
#include <string_view>

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

namespace nc::graphics
{
constexpr auto SwapChainColorRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto SwapChainDepthRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;

struct SinkTargets
{
    std::array<uint32_t, 4> color = std::array<uint32_t, 4>{0u};
    std::array<uint32_t, 4> depth = std::array<uint32_t, 4>{0u};
};

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
    SinkTargets sources = SinkTargets{};
    std::pair<uint32_t, uint32_t> sinks = std::make_pair(0u, 0u);
};
} // namespace nc::graphics
