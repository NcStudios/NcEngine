#pragma once

#include "MaterialDataBufferResource.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>

namespace nc::graphics
{
class MaterialResourceSignature
{
    public:
        explicit MaterialResourceSignature(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           std::string_view signatureName,
                                           std::string_view materialBufferVariableName,
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

        auto GetMaterialDataResource() -> MaterialDataBufferResource&
        {
            return *m_materialDataResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<MaterialDataBufferResource> m_materialDataResource;
};
} // namespace nc::graphics
