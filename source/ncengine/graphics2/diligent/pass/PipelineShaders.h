#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/Shader.h"

#include <string_view>
#include <unordered_map>

namespace nc::graphics
{
class ShaderFactory;

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

class ShaderCache
{
    public:
        explicit ShaderCache(ShaderFactory& shaderFactory);

        void Cache(std::string_view path,
                   Diligent::RefCntAutoPtr<Diligent::IShader> shader)
        {
            m_shaders.emplace(path, std::move(shader));
        }

        auto IsCached(std::string_view path) const -> bool                { return m_shaders.contains(path); }
        auto Get(std::string_view path)      const -> Diligent::IShader*;
        auto Get(const ShaderPaths& paths)   const -> PipelineShaders;

    private:
        using map = std::unordered_map<std::string_view, Diligent::RefCntAutoPtr<Diligent::IShader>>;
        map m_shaders;
};

namespace shader
{
constexpr auto NormalsPixel                = std::string_view{"NormalsPixel.spv"};
constexpr auto ParticleVertex              = std::string_view{"ParticleVertex.spv"};
constexpr auto ParticlePixel               = std::string_view{"ParticlePixel.spv"};
constexpr auto PointShadowMapPixel         = std::string_view{"PointShadowMapPixel.spv"};
constexpr auto PointShadowMapVertex        = std::string_view{"PointShadowMapVertex.spv"};
constexpr auto PointShadowMapSkinnedVertex = std::string_view{"PointShadowMapSkinnedVertex.spv"};
constexpr auto PostProcessVertex           = std::string_view{"PostProcessVertex.spv"};
constexpr auto PPEndPixel                  = std::string_view{"PPEndPixel.spv"};
constexpr auto PPFxaaPixel                 = std::string_view{"PPFxaaPixel.spv"};
constexpr auto PPNoisePixel                = std::string_view{"PPNoisePixel.spv"};
constexpr auto PPOutlinePixel              = std::string_view{"PPOutlinePixel.spv"};
constexpr auto ToonVertex                  = std::string_view{"ToonVertex.spv"};
constexpr auto ToonSkinnedVertex           = std::string_view{"ToonSkinnedVertex.spv"};
constexpr auto ToonPixel                   = std::string_view{"ToonPixel.spv"};
constexpr auto UniShadowMapVertex          = std::string_view{"UniShadowMapVertex.spv"};
constexpr auto UniShadowMapSkinnedVertex   = std::string_view{"UniShadowMapSkinnedVertex.spv"};
constexpr auto WireframeVertex             = std::string_view{"WireframeVertex.spv"};
constexpr auto WireframePixel              = std::string_view{"WireframePixel.spv"};
} // namespace shader
} // namespace nc::graphics
