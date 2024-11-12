#pragma once

#include "base/StructuredBuffer.h"

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
                                           uint32_t maxInstanceCount,
                                           uint32_t initialInstanceCount);

        void Commit(Diligent::IDeviceContext& context)
        {
            context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }

        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature&
        {
            return *m_signature;
        }

        auto GetMaterialDataResource() -> StructuredBuffer<MaterialData>&
        {
            return *m_materialDataResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<StructuredBuffer<MaterialData>> m_materialDataResource;
};
} // namespace nc::graphics
