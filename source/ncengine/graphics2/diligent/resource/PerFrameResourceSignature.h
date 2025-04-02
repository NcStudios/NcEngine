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
class CubeMapBufferResource;
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
                                           const StructuredBufferDesc& transformResourceDesc,
                                           const StructuredBufferDesc& staticMeshInstanceResourceDesc,
                                           const StructuredBufferDesc& skinnedMeshInstanceResourceDesc,
                                           const StructuredBufferDesc& lightResourceDesc,
                                           const StructuredBufferDesc& materialResourceDesc,
                                           const StructuredBufferDesc& boneResourceDesc,
                                           const StructuredBufferDesc& particleResourceDesc,
                                           const StructuredBufferDesc& lightMatrixResourceDesc,
                                           const CubeMapBufferDesc& cubeResourceDesc,
                                           const TextureBufferDesc& textureResourceDesc,
                                           const UniformBufferDesc& environmentResourceDesc,
                                           const UniformBufferDesc& wireframeResourceDesc);
        ~PerFrameResourceSignature() noexcept;

        void Commit(Diligent::IDeviceContext& context) { context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION); }
        auto GetResourceSignature()     -> Diligent::IPipelineResourceSignature&   { return *m_signature; }

        /* Resource Buffers */
        auto GetTransformBuffer()           -> StructuredBuffer<TransformData>&           { return *m_transformResource; }
        auto GetStaticMeshInstanceBuffer()  -> StructuredBuffer<StaticMeshInstanceData>&  { return *m_staticMeshInstanceResource; }
        auto GetSkinnedMeshInstanceBuffer() -> StructuredBuffer<SkinnedMeshInstanceData>& { return *m_skinnedMeshInstanceResource; }
        auto GetLightBuffer()               -> StructuredBuffer<LightData>&               { return *m_lightDataResource; }
        auto GetMaterialDataResource()      -> StructuredBuffer<MaterialData>&            { return *m_materialDataResource; }
        auto GetBoneDataResource()          -> StructuredBuffer<BoneData>&                { return *m_boneDataResource; }
        auto GetParticleDataBuffer()        -> StructuredBuffer<ParticleData>&            { return *m_particleDataResource; }
        auto GetLightMatrixDataBuffer()     -> StructuredBuffer<LightMatrixData>&         { return *m_lightMatrixDataResource; }
        auto GetCubeMapBuffer()             -> CubeMapBufferResource&                     { return *m_cubeMapResource; }
        auto GetTextureBuffer()             -> TextureBufferResource&                     { return *m_textureResource; }
        auto GetEnvironmentBuffer()         -> EnvironmentBufferResource&                 { return *m_environmentResource; }
        auto GetWireframeBuffer()           -> WireframeBufferResource&                   { return *m_wireframeBufferResource; }

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
        std::unique_ptr<StructuredBuffer<LightMatrixData>> m_lightMatrixDataResource;
        std::unique_ptr<TextureBufferResource> m_textureResource;
        std::unique_ptr<CubeMapBufferResource> m_cubeMapResource;
        std::unique_ptr<EnvironmentBufferResource> m_environmentResource;
        std::unique_ptr<WireframeBufferResource> m_wireframeBufferResource;
};
} // namespace nc::graphics
