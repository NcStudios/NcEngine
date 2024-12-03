#pragma once

#include "PostProcessPass.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class PerPassResourceSignature;
class PostProcessPropertyBufferResource;
struct PostProcessState;

class PostProcessPassBackend
{
    public:
        explicit PostProcessPassBackend(std::vector<PostProcessPipeline> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Update(Diligent::IDeviceContext& context,
                    const PostProcessState& postProcessState);

        void Render(Diligent::IDeviceContext& context,
                    Diligent::ISwapChain& swapChain,
                    PerPassResourceSignature& perPassResourceSignature,
                    PostProcessPropertyBufferResource& resource);

    private:
        std::vector<PostProcessPipeline> m_passes;
};
} // namespacae nc::graphics
