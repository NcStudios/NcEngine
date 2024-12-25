#pragma once

#include "PassManifest.h"
#include "ncengine/graphics/Material.h"
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
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex,
                          bool isMsaa_ = true)
        : pso{std::move(state)},
          id{passId},
          colorRTIndex{colorRTIndex_},
          depthRTIndex{depthRTIndex_},
          isMsaa{isMsaa_}
    {
    }

    explicit MaterialPass(Diligent::IRenderDevice& device,
                          const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                          MaterialPassFlag::type passId,
                          uint32_t colorRTIndex_ = SwapChainColorRTIndex,
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex,
                          bool isMsaa_ = true);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPassFlag::type id;
    uint32_t colorRTIndex;
    uint32_t depthRTIndex;
    bool isMsaa;
};

auto MakeMaterialPasses(Diligent::IRenderDevice& device,
                        ShaderFactory& shaderFactory,
                        ShaderBindings& shaderBindings,
                        std::span<const PassDesc> passManifest,
                        uint32_t numSamples = 1u)-> std::vector<MaterialPass>;
} // namespace nc::graphics
