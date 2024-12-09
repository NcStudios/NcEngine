#pragma once

#include "PassManifest.h"
#include "ncengine/graphics/Material.h"
#include "graphics2/diligent/resource/PostProcessSinkBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;

struct MaterialPass
{
    explicit MaterialPass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> state,
                          MaterialPassFlag::type passId,
                          uint32_t colorRTIndex_ = SwapChainColorRTIndex,
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex)
        : pso{std::move(state)},
          id{passId},
          colorRTIndex{colorRTIndex_},
          depthRTIndex{depthRTIndex_}
    {
    }

    explicit MaterialPass(Diligent::IRenderDevice& device,
                          const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                          MaterialPassFlag::type passId,
                          uint32_t colorRTIndex_ = SwapChainColorRTIndex,
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPassFlag::type id;
    uint32_t colorRTIndex;
    uint32_t depthRTIndex;
};

// todo 794 This only creates a Toon pass. Add more as they are implemented.
auto MakeMaterialPasses(Diligent::IRenderDevice& device,
                        ShaderFactory& shaderFactory,
                        ShaderBindings& shaderBindings,
                        std::span<const PassDesc> passManifest)-> std::vector<MaterialPass>;
} // namespace nc::graphics
