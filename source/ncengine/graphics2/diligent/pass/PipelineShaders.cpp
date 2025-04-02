#include "PipelineShaders.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "ncutility/NcError.h"

#include <array>

namespace
{
using namespace nc::graphics::shader;

constexpr auto g_pixelShaderPaths = std::array{
    NormalsPixel,
    ParticlePixel,
    PointShadowMapPixel,
    PPEndPixel,
    PPFxaaPixel,
    PPNoisePixel,
    PPOutlinePixel,
    SkyboxPixel,
    ToonPixel,
    WireframePixel
};

constexpr auto g_vertexShaderPaths = std::array{
    ParticleVertex,
    PointShadowMapVertex,
    PointShadowMapSkinnedVertex,
    PostProcessVertex,
    SkyboxVertex,
    ToonVertex,
    ToonSkinnedVertex,
    UniShadowMapVertex,
    UniShadowMapSkinnedVertex,
    WireframeVertex
};
} // anonymous namespace

namespace nc::graphics
{
auto ShaderCache::Get(std::string_view path) const -> Diligent::IShader*
{
    if (path.empty())
    {
        return nullptr;
    }

    NC_ASSERT(IsCached(path), fmt::format("Shader not loaded '{}'", path));
    return m_shaders.at(path).RawPtr();
}

auto ShaderCache::Get(const ShaderPaths& paths) const -> PipelineShaders
{
    return PipelineShaders{
        .pixelShader = Get(paths.pixelShaderPath),
        .vertexShader = Get(paths.vertexShaderPath)
    };
}

ShaderCache::ShaderCache(ShaderFactory& shaderFactory)
{
    m_shaders.reserve(g_pixelShaderPaths.size() + g_vertexShaderPaths.size());
    for (const auto& path : g_pixelShaderPaths)
    {
        auto bytecode = shaderFactory.ReadShaderFile(path);
        Cache(path, shaderFactory.MakeShaderFromByteCode(bytecode, path, Diligent::SHADER_TYPE_PIXEL));
    }

    for (const auto& path : g_vertexShaderPaths)
    {
        auto bytecode = shaderFactory.ReadShaderFile(path);
        Cache(path, shaderFactory.MakeShaderFromByteCode(bytecode, path, Diligent::SHADER_TYPE_VERTEX));
    }
}
} // namespace nc::graphics
