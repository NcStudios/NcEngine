#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"

namespace nc::graphics
{
auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PPPass>
{
    (void)device;
    (void)swapChain;
    (void)shaderFactory;
    return std::vector<PPPass>{};
}
} // namespace nc::graphics
