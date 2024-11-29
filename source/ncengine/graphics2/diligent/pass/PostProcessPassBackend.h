#pragma once

#include "graphics2/diligent/resource/base/UniformBuffer.h"
#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class PostProcessBufferResource;
struct PostProcessState;

struct PPPassInstanceData
{
    std::optional<UniformBuffer> buffer;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// oops, name conflicts w/ flags struct
struct PPPass
{
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    std::vector<PPPassInstanceData> instances;
    PostProcessPass::type id = PostProcessPass::None;
    bool anyEnabled = false;
};

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
