#pragma once

#include "ResourceTypes.h"
#include "base/StructuredBuffer.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>

namespace nc::graphics
{
class SinkBufferResource;
class SinkIndexBufferResource;

class PerPassResourceSignature
{
    constexpr auto PostProcessResourcePrefix = ""

    public:
        explicit PerPassResourceSignature(Diligent::IRenderDevice& device,
                                          Diligent::IDeviceContext& context,
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const TextureBufferResourceDesc& colorRTResourceDesc,
                                          const TextureBufferResourceDesc& depthRTResourceDesc,
                                          const UniformBufferResourceDesc& sinkIndexDesc);

        ~PerPassResourceSignature() noexcept;

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature()                  -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding()                    -> Diligent::IShaderResourceBinding&     { return *m_srb; }

        auto GetOffscreenColorResource()             -> SinkBufferResource&                   { return *m_offscreenColorResource; }
        auto GetOffscreenDepthResource()             -> SinkBufferResource&                   { return *m_offscreenDepthResource; }
        auto GetSinkIndexBufferResource()            -> SinkIndexBufferResource&              { return *m_sinkIndexBufferResource; }
        auto GetPostProcessResource(uint32_t index)  -> SinkBufferResource&                   { return m_postProcessResources.at(index); }
        void AddPostProcessResources(uint32_t count);

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<SinkBufferResource> m_offscreenColorResource;
        std::unique_ptr<SinkBufferResource> m_offscreenDepthResource;
        std::vector<SinkBufferResource> m_postProcessResources;
        std::unique_ptr<SinkIndexBufferResource> m_sinkIndexBufferResource;
};
} // namespace nc::graphics
