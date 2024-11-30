#pragma once

#include "PostProcessPass.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class PostProcessBufferResource;
struct PostProcessState;

class PostProcessPassBackend
{
    public:
        explicit PostProcessPassBackend(std::vector<PPPass> passes)
            : m_passes{std::move(passes)}
        {
        }

        void Update(Diligent::IDeviceContext& contet,
                    const PostProcessState& postProcessState);

        void Render(Diligent::IDeviceContext& context,
                    PostProcessBufferResource& resource);

    private:
        std::vector<PPPass> m_passes;
};
} // namespacae nc::graphics
