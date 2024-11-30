#pragma once

#include "graphics2/diligent/resource/base/UniformBuffer.h"
#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class ShaderFactory;

// Post process pass data specific to an effect
struct PPPassInstanceData
{
    std::optional<UniformBuffer> buffer;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// oops, name conflicts w/ flags struct
struct PPPass
{
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    std::vector<PPPassInstanceData> instances;
    PostProcessPass::type id = PostProcessPass::None;
    bool anyEnabled = false;
};

/** @todo This builds no passes */
auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PPPass>;
} // namespace nc::graphics
