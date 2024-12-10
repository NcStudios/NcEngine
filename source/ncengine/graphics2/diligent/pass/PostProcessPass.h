#pragma once

#include "PassManifest.h"
#include "graphics2/diligent/resource/base/DynamicUniformBuffer.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;
class PostProcessColorSinkBufferResource;

// Post process pass data specific to an effect
struct PostProcessPipelineInstance
{
    std::optional<DynamicUniformBuffer> buffer;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// Post process pass data shared by potentially many effects
struct PostProcessPass
{
    PostProcessPass(Diligent::IRenderDevice& device,
                    const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                    std::vector<PostProcessPipelineInstance> instances_,
                    PassDesc passDesc);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    std::vector<PostProcessPipelineInstance> instances;
    PassDesc passDesc;
    bool anyEnabled = false;
};

auto MakePostProcessPass(Diligent::IRenderDevice& device,
                         Diligent::IDeviceContext& context,
                         Diligent::ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         PassDesc passDesc) -> PostProcessPass;

auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::IDeviceContext& context,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest) -> std::vector<PostProcessPass>;
} // namespace nc::graphics
