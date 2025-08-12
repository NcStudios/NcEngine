#pragma once

#include "ResourceTypes.h"
#include "base/StructuredBuffer.h"
#include "CubeSinkBufferResource.h"
#include "PostProcessPropertyBufferResource.h"
#include "SinkBufferResource.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>

namespace nc::graphics
{
class SinkIndexBufferResource;

class PerPassResourceSignature
{
    public:
        explicit PerPassResourceSignature(Diligent::IRenderDevice& device,
                                          Diligent::IDeviceContext& context,
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const SinkBufferDesc& colorSinksDesc,
                                          const SinkBufferDesc& depthSinksDesc,
                                          const SinkBufferDesc& postProcessSinksDesc,
                                          const SinkBufferDesc& uniShadowMapSinksDesc,
                                          const CubeSinkBufferDesc& pointShadowMapSinksDesc,
                                          const UniformBufferDesc& postProcessResourceDesc,
                                          const UniformBufferDesc& sinkIndexDesc);

        ~PerPassResourceSignature() noexcept;

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding()   -> Diligent::IShaderResourceBinding&     { return *m_srb; }
        void BindPostProcessSink(uint32_t index);

        auto GetColorSinksResource()                    -> SinkBufferResource&                { return *m_colorSinksResource; }
        auto GetDepthSinksResource()                    -> SinkBufferResource&                { return *m_depthSinksResource; }
        auto GetSinkIndexBufferResource()               -> SinkIndexBufferResource&           { return *m_sinkIndexBufferResource; }
        auto GetPostProcessSinkResource(uint32_t index) -> SinkBufferResource&                { return m_postProcessSinksResource.at(index); }
        auto GetPostProcessPropertyResource()           -> PostProcessPropertyBufferResource& { return *m_postProcessPropertyResource; }
        auto GetUniShadowMapSinksResource()             -> SinkBufferResource&                { return *m_uniShadowMapSinksResource; }
        auto GetPointShadowMapSinksResource()           -> CubeSinkBufferResource&            { return *m_pointShadowMapSinksResource; }

        auto GetPostProcessSinkCount() const      -> uint32_t { return m_postProcessSinkCount; }
        auto GetColorSinkCount(bool isMsaa) const -> uint32_t { return isMsaa ? m_colorSinksResource->GetMsaaSinkCount() : m_colorSinksResource->GetSinkCount(); }
        auto GetDepthSinkCount(bool isMsaa) const -> uint32_t { return isMsaa ? m_depthSinksResource->GetMsaaSinkCount() : m_depthSinksResource->GetSinkCount(); }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<SinkBufferResource> m_colorSinksResource;
        std::unique_ptr<SinkBufferResource> m_depthSinksResource;
        std::vector<SinkBufferResource> m_postProcessSinksResource;
        std::vector<TextureBufferDesc> m_postProcessSinkDescs;
        std::unique_ptr<SinkIndexBufferResource> m_sinkIndexBufferResource;
        std::unique_ptr<PostProcessPropertyBufferResource> m_postProcessPropertyResource;
        std::unique_ptr<SinkBufferResource> m_uniShadowMapSinksResource;
        std::unique_ptr<CubeSinkBufferResource> m_pointShadowMapSinksResource;
        uint32_t m_postProcessSinkCount;
        std::string m_postProcessResourceKey;
};
} // namespace nc::graphics
