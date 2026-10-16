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
    LoadOp colorLoadOp = LoadOp::DontCare;
    LoadOp depthLoadOp = LoadOp::DontCare;
    LoadOp stencilLoadOp = LoadOp::DontCare;
    CullMode cullMode = CullMode::Back;
    IsMsaa isMsaa = IsMsaa{true};
    bool useDepthTest = true;
    bool alphaBlend = false;
    bool writeStencil = false;
    bool useStencilTest = false;
};
} // namespace nc::graphics