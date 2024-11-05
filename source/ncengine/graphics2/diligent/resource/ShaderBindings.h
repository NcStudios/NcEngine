#pragma once

#include "GlobalMeshBuffer.h"
#include "component/ComponentResourceSignature.h"
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
                                uint32_t maxTextures,
                                uint32_t maxMeshRenderers)
            : m_componentSignature{device, context, maxMeshRenderers}, 
              m_globalSignature{device, context, maxTextures}
        {
        }

        void Update(const FrontendRenderState& renderState, Diligent::IDeviceContext& context);

        auto GetComponentSignature() -> ComponentResourceSignature&
        {
            return m_componentSignature;
        }

        auto GetGlobalSignature() -> GlobalResourceSignature&
        {
            return m_globalSignature;
        }

        auto GetMeshBuffer() -> GlobalMeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        ComponentResourceSignature m_componentSignature;
        GlobalResourceSignature m_globalSignature;
        GlobalMeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
