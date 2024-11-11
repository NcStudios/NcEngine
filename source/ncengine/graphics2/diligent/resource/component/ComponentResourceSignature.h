#pragma once

#include "MeshRendererBufferResource.h"

#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <memory>

namespace nc::graphics
{
/*
Expand to hold ParticleEmitter, SkinnedRenderer, etc.
*/
class ComponentResourceSignature
{
    public:
        explicit ComponentResourceSignature(Diligent::IDeviceContext& context,
                                            Diligent::IRenderDevice& device,
                                            std::string_view signatureName,
                                            std::string_view meshRendererBufferVariableName,
                                            uint8_t bindingIndex,
                                            uint32_t maxInstances);

        void Commit(Diligent::IDeviceContext& context)
        {
            context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }

        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature&
        {
            return *m_signature;
        }

        auto GetMeshRendererBuffer() -> MeshRendererBufferResource&
        {
            return *m_meshRendererResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<MeshRendererBufferResource> m_meshRendererResource;
};
} // namespace nc::graphics
