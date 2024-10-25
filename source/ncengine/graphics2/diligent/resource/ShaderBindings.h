#pragma once

#include "GlobalResourceSignature.h"
#include "MeshBuffer.h"

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

        auto GetMeshBuffer() -> MeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        GlobalResourceSignature m_globalSignature;
        MeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
