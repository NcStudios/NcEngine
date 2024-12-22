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
    public:
        explicit PerPassResourceSignature(Diligent::IRenderDevice& device,
                                          Diligent::IDeviceContext& context,
                                          std::string_view signatureName,
                                          uint8_t bindingIndex,
                                          const TextureBufferResourceDesc& colorRTResourceDesc,
                                          const TextureBufferResourceDesc& depthRTResourceDesc,
                                          const UniformBufferResourceDesc& sinkIndexDesc);

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }

        auto GetResourceSignature()       -> Diligent::IPipelineResourceSignature& { return *m_signature; }
        auto GetResourceBinding()         -> Diligent::IShaderResourceBinding&     { return *m_srb; }
        auto GetColorSinkBufferResource() -> SinkBufferResource&                   { return *m_colorSinkBufferResource; }
        auto GetDepthSinkBufferResource() -> SinkBufferResource&                   { return *m_depthSinkBufferResource; }
        auto GetSinkIndexBufferResource() -> SinkIndexBufferResource&              { return *m_sinkIndexBufferResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<SinkBufferResource> m_colorSinkBufferResource;
        std::unique_ptr<SinkBufferResource> m_depthSinkBufferResource;
        std::unique_ptr<SinkIndexBufferResource> m_sinkIndexBufferResource;
};
} // namespace nc::graphics
