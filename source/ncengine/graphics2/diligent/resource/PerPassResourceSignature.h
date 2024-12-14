#pragma once

#include "ResourceTypes.h"
#include "base/StructuredBuffer.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>

namespace nc::graphics
{
class PostProcessColorSinkBufferResource;
class PostProcessDepthSinkBufferResource;
struct PostProcessSinkIndexBufferResource;

class PerPassResourceSignature
{
    public:
        explicit PerPassResourceSignature(Diligent::IRenderDevice& device,
                                          Diligent::IDeviceContext& context,
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const TextureBufferResourceDesc& postProcessColorRTResourceDesc,
                                          const TextureBufferResourceDesc& postProcessDepthRTResourceDesc,
                                          const UniformBufferResourceDesc& postProcessSinkIndexDesc);

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding() -> Diligent::IShaderResourceBinding& { return *m_srb; }

        auto GetPostProcessColorSinkBufferResource() -> PostProcessColorSinkBufferResource& { return *m_postProcessColorSinkBufferResource; }
        auto GetPostProcessDepthSinkBufferResource() -> PostProcessDepthSinkBufferResource& { return *m_postProcessDepthSinkBufferResource; }
        auto GetPostProcessSinkIndexBufferResource() -> PostProcessSinkIndexBufferResource& { return *m_postProcessSinkIndexBufferResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<PostProcessColorSinkBufferResource> m_postProcessColorSinkBufferResource;
        std::unique_ptr<PostProcessDepthSinkBufferResource> m_postProcessDepthSinkBufferResource;
        std::unique_ptr<PostProcessSinkIndexBufferResource> m_postProcessSinkIndexBufferResource;
};
} // namespace nc::graphics
