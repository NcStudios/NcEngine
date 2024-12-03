#include "MaterialPass.h"
#include "PassUtilities.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

namespace nc::graphics
{
MaterialPass::MaterialPass(Diligent::IRenderDevice& device,
           const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
           MaterialPassFlag::type passId,
           uint32_t colorRTIndex_,
           uint32_t depthRTIndex_)
    : pso{},
      id{passId},
      colorRTIndex{colorRTIndex_},
      depthRTIndex{depthRTIndex_}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakePasses(Diligent::IRenderDevice& device,
                Diligent::ISwapChain& swapChain,
                ShaderFactory& shaderFactory,
                ShaderBindings& shaderBindings) -> std::vector<MaterialPass>
{
    return std::vector<MaterialPass>{
        MakeOffScreenMaterialPass(
            device,
            swapChain,
            shaderFactory,
            shaderBindings.GetPerFrameSignature().GetResourceSignature(),
            shaderBindings.GetPerPassSignature().GetPostProcessSinkBufferResource(),
            "Toon.psh",
            "Toon.vsh",
            "Toon Pipeline"
        )
    };
}
} // namespace nc::graphics
