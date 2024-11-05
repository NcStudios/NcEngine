#pragma once

#include "MaterialPropertiesBufferResource.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class MaterialResourceSignature
{
    public:
        explicit MaterialResourceSignature(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           std::string_view signatureName,
                                           std::string_view materialPropertiesVariableName,
                                           uint8_t bindingIndex,
                                           uint32_t maxInstances);

        void Commit(Diligent::IDeviceContext& context)
        {
            context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }

        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature&
        {
            return *m_signature;
        }

        auto GetMaterialPropertiesResource() -> MaterialPropertiesBufferResource&
        {
            return *m_materialPropertiesResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<MaterialPropertiesBufferResource> m_materialPropertiesResource;
};
} // namespace nc::graphics
