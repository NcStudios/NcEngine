#pragma once

#include "GlobalMeshBuffer.h"
#include "component/ComponentResourceSignature.h"
#include "GlobalResourceSignature.h"
#include "MaterialResourceSignature.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

namespace nc::graphics
{
struct FrontendRenderState;

class ShaderBindings
{
    public:
        /** @todo: 782 Fully parameterize names, indices, and size hints */
        explicit ShaderBindings(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                uint32_t maxTextures,
                                uint32_t maxMeshRenderers,
                                uint32_t initialMaterialSizeHint)
            : m_globalSignature{device, context, maxTextures},
              m_componentSignature{device, context, maxMeshRenderers}, 
              m_materialSignature{
                context,
                device,
                "MaterialInstanceSignature",
                "MaterialDataBuffer",
                2,
                initialMaterialSizeHint
              }
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

        auto GetMaterialSignature() -> MaterialResourceSignature&
        {
            return m_materialSignature;
        }

        auto GetMeshBuffer() -> GlobalMeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        ComponentResourceSignature m_componentSignature;
        GlobalResourceSignature m_globalSignature;
        MaterialResourceSignature m_materialSignature;
        GlobalMeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
