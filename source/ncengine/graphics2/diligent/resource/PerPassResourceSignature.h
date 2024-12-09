#pragma once

#include "ResourceTypes.h"
#include "base/StructuredBuffer.h"
#include "PostProcessSinkBufferResource.h"
#include "PostProcessSinkIndexBufferResource.h"
#include "PostProcessPropertyBufferResource.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>

namespace nc::graphics
{
class PerPassResourceSignature
{
    public:
        explicit PerPassResourceSignature(Diligent::IRenderDevice& device,
                                          Diligent::IDeviceContext& context,
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const TextureBufferResourceDesc& postProcessResourceDesc,
                                          const UniformBufferResourceDesc& postProcessSinkIndexDesc,
                                          const DynamicUniformBufferResourceDesc& outlinePassPropertiesDesc);

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding() -> Diligent::IShaderResourceBinding& { return *m_srb; }

        auto GetPostProcessSinkBufferResource() -> PostProcessSinkBufferResource& { return *m_postProcessBufferResource; }
        auto GetPostProcessSinkIndexBufferResource() -> PostProcessSinkIndexBufferResource& { return *m_postProcessSinkIndexBufferResource; }
        auto GetPostProcessPropertyBuffer() -> PostProcessPropertyBufferResource&      { return *m_postProcessPropertyResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<PostProcessSinkBufferResource> m_postProcessBufferResource;
        std::unique_ptr<PostProcessSinkIndexBufferResource> m_postProcessSinkIndexBufferResource;
        std::unique_ptr<PostProcessPropertyBufferResource> m_postProcessPropertyResource;
};
} // namespace nc::graphics
