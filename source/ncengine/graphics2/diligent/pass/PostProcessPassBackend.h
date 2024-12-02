#pragma once

#include "PostProcessPass.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class PostProcessPropertyBufferResource;
struct PostProcessState;

class PostProcessPassBackend
{
    public:
        explicit PostProcessPassBackend(std::vector<PostProcessPipeline> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Update(Diligent::IDeviceContext& contet,
                    const PostProcessState& postProcessState);

        void Render(Diligent::IDeviceContext& context,
                    PostProcessPropertyBufferResource& resource);

    private:
        std::vector<PostProcessPipeline> m_passes;
};
} // namespacae nc::graphics
