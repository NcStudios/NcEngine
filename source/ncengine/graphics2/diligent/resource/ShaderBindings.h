#pragma once

#include "GlobalResourceSignature.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

namespace nc::graphics
{
struct FrontendRenderState;

class ShaderBindings
{
    public:
        explicit ShaderBindings(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                uint32_t maxTextures)
            : m_globalSignature{device, context, maxTextures}
        {
        }

        void Update(const FrontendRenderState& renderState, Diligent::IDeviceContext& context);

        auto GetGlobalSignature() -> GlobalResourceSignature&
        {
            return m_globalSignature;
        }

    private:
        GlobalResourceSignature m_globalSignature;
};
} // namespace nc::graphics
