#pragma once

#include "MeshBuffer.h"
#include "PerFrameResourceSignature.h"
#include "PerPassResourceSignature.h"
#include "base/StructuredBuffer.h"

#include "ncengine/config/Config.h"

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
                                const config::MemorySettings& memorySettings)
            : m_perFrameSignature{
                context, device,
                "PerFrameResourceSignature",
                0,
                StructuredBufferResourceDesc{"TransformBufferData",        Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,         memorySettings.maxRenderers / 2},
                StructuredBufferResourceDesc{"StaticInstanceBufferData",   Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,         memorySettings.maxRenderers / 2},
                StructuredBufferResourceDesc{"SkinnedInstanceBufferData",  Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,         memorySettings.maxRenderers / 2},
                StructuredBufferResourceDesc{"LightBufferData",            Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  GetTotalLightCount(memorySettings),  GetTotalLightCount(memorySettings)},
                StructuredBufferResourceDesc{"MaterialBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,  memorySettings.maxRenderers,         memorySettings.maxRenderers / 2},
                StructuredBufferResourceDesc{"BoneBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, memorySettings.maxBones,             memorySettings.maxBones / 4},
                StructuredBufferResourceDesc{"ParticleBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,  memorySettings.maxParticles,         memorySettings.maxParticles / 4},
                TextureBufferResourceDesc{"TextureBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,  memorySettings.maxTextures},
                UniformBufferResourceDesc{"EnvironmentBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"WireframeBufferData",           Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"OutlinePassBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL}
              },
              m_perPassSignature{
                device, context,
                "PerPassResourceSignature",
                1,
                TextureBufferResourceDesc{"PostProcessColorSinkBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, 5},
                TextureBufferResourceDesc{"PostProcessDepthSinkBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, 2},
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

        static auto GetTotalLightCount(const config::MemorySettings& settings) -> uint32_t
        {
            return settings.maxDirectionalLights +
                   settings.maxPointLights       +
                   settings.maxSpotLights;
        }
};
} // namespace nc::graphics
