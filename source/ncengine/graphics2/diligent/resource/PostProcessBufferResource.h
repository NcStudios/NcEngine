#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/graphics/PostProcess.h"
#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct PostProcessState;

// Not sure if this should be its own thing, or in EnvironmentBufferResource...
class PostProcessBufferResource
{
    public:
        explicit PostProcessBufferResource(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IShaderResourceVariable& outlineDataVariable);

        void Update(Diligent::IDeviceContext& context,
                    PostProcessPass::type passId,
                    std::span<const char* const> data);

        auto GetOutlineDataShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_outlineDataVariable;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_outlineDataBuffer;
        Diligent::IShaderResourceVariable* m_outlineDataVariable;

        auto GetBuffer(PostProcessPass::type passId) -> Diligent::IBuffer&
        {
            switch (passId)
            {
                case PostProcessPass::Outline: return *m_outlineDataBuffer;
                default:
                    NC_ASSERT(false, fmt::format("No variable for PostProcessPass '{}'", passId));
                    std::unreachable();
            }
        }
};
} // namespace nc::graphics
