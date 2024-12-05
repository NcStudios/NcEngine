#pragma once

#include "MaterialPass.h"
#include "MaterialPassBackend.h"
#include "PostProcessPass.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class PostProcessPropertyBufferResource;

class PassBackend
{
    public:
        explicit PassBackend(std::vector<MaterialPass> materialPasses, std::vector<PostProcessPass> postProcessPasses)
            : m_materialPassBackend{std::move(materialPasses)},
              m_postProcessPasses{std::move(postProcessPasses)}{}

        void Update(Diligent::IDeviceContext& context, const PostProcessState& postProcessState);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const std::vector<std::vector<Batch>>& passBatches) { m_materialPassBackend.Render(context, swapChain, perPassResourceSignature, passBatches); }

        void RenderPostProcess(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain,
                               PerPassResourceSignature& perPassResourceSignature,
                               PostProcessPropertyBufferResource& resource);

    private:
        // std::vector<MaterialPass> m_materialPasses;
        std::vector<PostProcessPass> m_postProcessPasses;
        MaterialPassBackend m_materialPassBackend;
};
} // namespace nc::graphics
