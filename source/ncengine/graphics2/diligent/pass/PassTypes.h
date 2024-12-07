#pragma once

#include <span>
#include <string_view>
#include <vector>

namespace nc::graphics
{
struct RenderTargets
{
    std::vector<uint32_t> colorIndices;
    std::vector<uint32_t> depthIndices;
};

struct ShaderPaths
{
    std::string_view pixelShaderPath;
    std::string_view vertexShaderPath;
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
        PassType passType;
        ShaderPaths shaderPaths;
        RenderTargets sources;
        RenderTargets sinks;
};
} // namespace nc::graphics
