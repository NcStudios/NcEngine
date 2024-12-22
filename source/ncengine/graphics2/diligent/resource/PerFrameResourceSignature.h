#pragma once

#include "graphics2/diligent/resource/base/StructuredBuffer.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/ShaderTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <memory>

namespace nc::graphics
{
class TextureBufferResource;
class EnvironmentBufferResource;
class PostProcessPropertyBufferResource;
class WireframeBufferResource;

class PerFrameResourceSignature
{
    public:
        explicit PerFrameResourceSignature(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           std::string_view signatureName,
                                           uint8_t bindingIndex,
                                           const StructuredBufferResourceDesc& transformResourceDesc,
                                           const StructuredBufferResourceDesc& staticMeshInstanceResourceDesc,
                                           const StructuredBufferResourceDesc& skinnedMeshInstanceResourceDesc,
                                           const StructuredBufferResourceDesc& lightResourceDesc,
                                           const StructuredBufferResourceDesc& materialResourceDesc,
                                           const StructuredBufferResourceDesc& boneResourceDesc,
                                           const StructuredBufferResourceDesc& particleResourceDesc,
                                           const TextureBufferResourceDesc& textureResourceDesc,
                                           const UniformBufferResourceDesc& environmentResourceDesc,
                                           const UniformBufferResourceDesc& wireframeResourceDesc,
                                           const UniformBufferResourceDesc& outlinePassPropertiesDesc);
        ~PerFrameResourceSignature() noexcept;

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY); }
        auto GetResourceSignature()     -> Diligent::IPipelineResourceSignature&   { return *m_signature; }

        /* Resource Buffers */
        auto GetTransformBuffer()           -> StructuredBuffer<TransformData>&           { return *m_transformResource; }
        auto GetStaticMeshInstanceBuffer()  -> StructuredBuffer<StaticMeshInstanceData>&  { return *m_staticMeshInstanceResource; }
        auto GetSkinnedMeshInstanceBuffer() -> StructuredBuffer<SkinnedMeshInstanceData>& { return *m_skinnedMeshInstanceResource; }
        auto GetLightBuffer()               -> StructuredBuffer<LightData>&               { return *m_lightDataResource; }
        auto GetMaterialDataResource()      -> StructuredBuffer<MaterialData>&            { return *m_materialDataResource; }
        auto GetBoneDataResource()          -> StructuredBuffer<BoneData>&                { return *m_boneDataResource; }
        auto GetParticleDataBuffer()        -> StructuredBuffer<ParticleData>&            { return *m_particleDataResource; }
        auto GetTextureBuffer()             -> TextureBufferResource&                     { return *m_textureResource; }
        auto GetEnvironmentBuffer()         -> EnvironmentBufferResource&                 { return *m_environmentResource; }
        auto GetWireframeBuffer()           -> WireframeBufferResource&                   { return *m_wireframeBufferResource; }
        auto GetPostProcessPropertyBuffer() -> PostProcessPropertyBufferResource&         { return *m_postProcessPropertyResource; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<StructuredBuffer<TransformData>> m_transformResource;
        std::unique_ptr<StructuredBuffer<StaticMeshInstanceData>> m_staticMeshInstanceResource;
        std::unique_ptr<StructuredBuffer<SkinnedMeshInstanceData>> m_skinnedMeshInstanceResource;
        std::unique_ptr<StructuredBuffer<LightData>> m_lightDataResource;
        std::unique_ptr<StructuredBuffer<MaterialData>> m_materialDataResource;
        std::unique_ptr<StructuredBuffer<BoneData>> m_boneDataResource;
        std::unique_ptr<StructuredBuffer<ParticleData>> m_particleDataResource;
        std::unique_ptr<TextureBufferResource> m_textureResource;
        std::unique_ptr<EnvironmentBufferResource> m_environmentResource;
        std::unique_ptr<WireframeBufferResource> m_wireframeBufferResource;
        std::unique_ptr<PostProcessPropertyBufferResource> m_postProcessPropertyResource;
};
} // namespace nc::graphics
