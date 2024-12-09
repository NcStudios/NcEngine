#pragma once

#include <span>
#include <string_view>
#include <vector>

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"

namespace nc::graphics
{
constexpr auto SwapChainColorRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto SwapChainDepthRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;

struct RenderTargets
{
    std::vector<uint32_t> colorIndices = std::vector<uint32_t>{};
    std::vector<uint32_t> depthIndices = std::vector<uint32_t>{};
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
    RenderTargets sources = RenderTargets{};
    std::pair<uint32_t, uint32_t> sinks = std::make_pair(0u, 0u);
};
} // namespace nc::graphics
