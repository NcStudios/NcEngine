#pragma once

#include "ResourceTypes.h"
#include "base/StructuredBuffer.h"
#include "PostProcessSinkBufferResource.h"

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
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const TextureBufferResourceDesc& postProcessResourceDesc);

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding() -> Diligent::IShaderResourceBinding& { return *m_srb; }

        auto GetPostProcessBufferResource() -> PostProcessSinkBufferResource& { return *m_postProcessBufferResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<PostProcessSinkBufferResource> m_postProcessBufferResource;
};
} // namespace nc::graphics
