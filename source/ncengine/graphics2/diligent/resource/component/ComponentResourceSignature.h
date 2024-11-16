#pragma once

#include "graphics2/diligent/resource/base/StructuredBuffer.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/ShaderTypes.h"
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
                                            uint8_t bindingIndex,
                                            StructuredBufferResourceDesc meshRendererResourceDesc,
                                            StructuredBufferResourceDesc directionalLightResourceDesc,
                                            StructuredBufferResourceDesc pointLightResourceDesc,
                                            StructuredBufferResourceDesc spotLightResourceDesc);

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY); }
        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature& { return *m_signature; }

        /* Resource Buffers */
        auto GetMeshRendererBuffer()    -> StructuredBuffer<MeshRendererData>&     { return *m_meshRendererResource; }
        auto GetDirectionaLightBuffer() -> StructuredBuffer<DirectionalLightData>& { return *m_directionalLightResource; }
        auto GetPointLightBuffer()      -> StructuredBuffer<PointLightData>&       { return *m_pointLightResource; }
        auto GetSpotLightBuffer()       -> StructuredBuffer<SpotLightData>&        { return *m_spotLightResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<StructuredBuffer<MeshRendererData>> m_meshRendererResource;
        std::unique_ptr<StructuredBuffer<DirectionalLightData>> m_directionalLightResource;
        std::unique_ptr<StructuredBuffer<PointLightData>> m_pointLightResource;
        std::unique_ptr<StructuredBuffer<SpotLightData>> m_spotLightResource;
};
} // namespace nc::graphics
