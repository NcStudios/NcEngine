#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"

namespace nc::graphics
{
auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>
{
    (void)device;
    (void)swapChain;
    (void)shaderFactory;
    return std::vector<PostProcessPipeline>{};
}
} // namespace nc::graphics
