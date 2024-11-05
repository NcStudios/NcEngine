#pragma once

#include "GlobalMeshBuffer.h"
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
                                uint32_t initialMaterialSizeHint)
            : m_globalSignature{device, context, maxTextures},
              m_materialSignature{
                context,
                device,
                "MaterialInstanceSignature",
                "MaterialPropertiesBuffer",
                1,
                initialMaterialSizeHint
              }
        {
        }

        void Update(const FrontendRenderState& renderState, Diligent::IDeviceContext& context);

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
        GlobalResourceSignature m_globalSignature;
        MaterialResourceSignature m_materialSignature;
        GlobalMeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
