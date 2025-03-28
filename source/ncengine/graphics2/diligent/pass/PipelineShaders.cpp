#include "PipelineShaders.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "ncutility/NcError.h"

#include <array>

namespace
{
constexpr auto g_fragmentShaderPaths = std::array{
    nc::graphics::shader::NormalsFragment,
    nc::graphics::shader::ParticleFragment,
    nc::graphics::shader::PointShadowMapFragment,
    nc::graphics::shader::PPEndFragment,
    nc::graphics::shader::PPFxaaFragment,
    nc::graphics::shader::PPNoiseFragment,
    nc::graphics::shader::PPOutlineFragment,
    nc::graphics::shader::ToonFragment,
    nc::graphics::shader::WireframeFragment
};

constexpr auto g_vertexShaderPaths = std::array{
    nc::graphics::shader::ParticleVertex,
    nc::graphics::shader::PointShadowMapVertex,
    nc::graphics::shader::PointShadowMapSkinnedVertex,
    nc::graphics::shader::PostProcessVertex,
    nc::graphics::shader::ToonVertex,
    nc::graphics::shader::ToonSkinnedVertex,
    nc::graphics::shader::UniShadowMapVertex,
    nc::graphics::shader::UniShadowMapSkinnedVertex,
    nc::graphics::shader::WireframeVertex
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
    m_shaders.reserve(g_fragmentShaderPaths.size() + g_vertexShaderPaths.size());
    for (const auto& path : g_fragmentShaderPaths)
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
