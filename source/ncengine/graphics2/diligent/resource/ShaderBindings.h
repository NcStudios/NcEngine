#pragma once

#include "MeshBuffer.h"
#include "PerFrameResourceSignature.h"
#include "PerPassResourceSignature.h"
#include "base/StructuredBuffer.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

namespace nc::graphics
{
struct FrontendRenderState;

class ShaderBindings
{
    public:
        explicit ShaderBindings(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                uint32_t maxTextures,
                                uint32_t maxMeshRenderers,
                                uint32_t maxSpotLights,
                                uint32_t maxPointLights,
                                uint32_t maxDirectionalLights,
                                uint32_t maxBones,
                                uint32_t maxParticles,
                                uint32_t initialInstanceSizeHint,
                                uint32_t initialMaterialSizeHint,
                                uint32_t initialBonesSizeHint)
            : m_perFrameSignature{
                context, device,
                "PerFrameResourceSignature",
                0,
                StructuredBufferResourceDesc{"TransformBufferData",        Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, maxMeshRenderers,     initialInstanceSizeHint},
                StructuredBufferResourceDesc{"StaticInstanceBufferData",   Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, maxMeshRenderers,     initialInstanceSizeHint},
                StructuredBufferResourceDesc{"SkinnedInstanceBufferData",  Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, maxMeshRenderers,     initialInstanceSizeHint},
                StructuredBufferResourceDesc{"DirectionalLightBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  maxDirectionalLights, maxDirectionalLights},
                StructuredBufferResourceDesc{"PointLightBufferData",       Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  maxPointLights,       maxPointLights},
                StructuredBufferResourceDesc{"SpotLightBufferData",        Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  maxSpotLights,        maxSpotLights},
                StructuredBufferResourceDesc{"MaterialBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,  maxMeshRenderers,     initialMaterialSizeHint},
                StructuredBufferResourceDesc{"BoneBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, maxBones,             initialBonesSizeHint},
                StructuredBufferResourceDesc{"ParticleBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  maxParticles,         maxParticles}, // initial size???
                TextureBufferResourceDesc{"TextureBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,  maxTextures},
                UniformBufferResourceDesc{"EnvironmentBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"WireframeBufferData",           Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"OutlinePassBufferData",          Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL}
              },
              m_perPassSignature{
                device, context,
                "PerPassResourceSignature",
                1,
                TextureBufferResourceDesc{"PostProcessColorSinkBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, 10},
                TextureBufferResourceDesc{"PostProcessDepthSinkBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, 10},
                UniformBufferResourceDesc{"PostProcessSinkIndexBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL}
            }
        {
        }

        void Update(Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device,
                    const FrontendRenderState& renderState);

        auto GetPerFrameSignature() -> PerFrameResourceSignature&
        {
            return m_perFrameSignature;
        }

        auto GetPerPassSignature() -> PerPassResourceSignature&
        {
            return m_perPassSignature;
        }

        auto GetMeshBuffer() -> MeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        PerFrameResourceSignature m_perFrameSignature;
        PerPassResourceSignature m_perPassSignature;
        MeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
