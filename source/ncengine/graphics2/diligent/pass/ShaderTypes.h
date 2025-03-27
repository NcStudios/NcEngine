#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/Shader.h"

#include <array>
#include <span>
#include <string_view>
#include <unordered_map>

namespace nc::graphics
{
using ShaderMap = std::unordered_map<std::string_view, Diligent::RefCntAutoPtr<Diligent::IShader>>;

struct ShaderPaths
{
    std::string_view pixelShaderPath = "";
    std::string_view vertexShaderPath = "";
};

struct PipelineShaders
{
    Diligent::IShader* pixelShader = nullptr;
    Diligent::IShader* vertexShader = nullptr;
};

namespace shader
{
constexpr auto NormalsFragment        = std::string_view{"NormalsFragment.spv"};
constexpr auto ParticleVertex         = std::string_view{"ParticleVertex.spv"};
constexpr auto ParticleFragment       = std::string_view{"ParticleFragment.spv"};
constexpr auto PostProcessVertex      = std::string_view{"PostProcessVertex.spv"};
constexpr auto PPEndFragment          = std::string_view{"PPEndFragment.spv"};
constexpr auto PPFxaaFragment         = std::string_view{"PPFxaaFragment.spv"};
constexpr auto PPNoiseFragment        = std::string_view{"PPNoiseFragment.spv"};
constexpr auto PPOutlineFragment      = std::string_view{"PPOutlineFragment.spv"};
constexpr auto ShadowMapSkinnedVertex = std::string_view{"ShadowMapSkinnedVertex.spv"};
constexpr auto ShadowMapVertex        = std::string_view{"ShadowMapVertex.spv"};
constexpr auto ToonSkinnedVertex      = std::string_view{"ToonSkinnedVertex.spv"};
constexpr auto ToonVertex             = std::string_view{"ToonVertex.spv"};
constexpr auto ToonFragment           = std::string_view{"ToonFragment.spv"};
constexpr auto WireframeVertex        = std::string_view{"WireframeVertex.spv"};
constexpr auto WireframeFragment      = std::string_view{"WireframeFragment.spv"};
} // namespace shader

constexpr auto g_fragmentShaderPaths = std::array{
    shader::NormalsFragment,
    shader::ParticleFragment,
    shader::PPEndFragment,
    shader::PPFxaaFragment,
    shader::PPNoiseFragment,
    shader::PPOutlineFragment,
    shader::ToonFragment,
    shader::WireframeFragment
};

constexpr auto g_vertexShaderPaths = std::array{
    shader::ParticleVertex,
    shader::PostProcessVertex,
    shader::ShadowMapSkinnedVertex,
    shader::ShadowMapVertex,
    shader::ToonSkinnedVertex,
    shader::ToonVertex,
    shader::WireframeVertex
};
} // namespace nc::graphics
