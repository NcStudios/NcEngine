#pragma once

#include "PassTypes.h"

namespace nc::graphics
{
struct PassDesc
{
    uint64_t flag = 0;
    std::string_view name  = "";
    PassType type = PassType::None;
    VertexAttribute layoutElements = VertexAttribute::None;
    ShaderPaths shaderPaths = ShaderPaths{};
    std::vector<ColorTarget> colorSources = std::vector<ColorTarget>{};
    std::vector<DepthTarget> depthSources = std::vector<DepthTarget>{};
    PostProcessTarget postProcessSource = PostProcessTarget::None;
    ColorTarget colorSink = ColorTarget::None;
    DepthTarget depthSink = DepthTarget::None;
    ShadowMapTarget shadowMapSink = ShadowMapTarget::None;
    PostProcessTarget postProcessSink = PostProcessTarget::None;
    uint8_t numRenderTargets = 0;
    CullMode cullMode = CullMode::Back;
    IsMsaa isMsaa = IsMsaa{true};
    UseDepthTest useDepthTest = UseDepthTest{true};
    IsAlphaBlend isAlphaBlend = IsAlphaBlend{false};
    WriteStencil writeStencil = WriteStencil{false};
    UseStencilTest useStencilTest = UseStencilTest{false};
};
} // namespace nc::graphics