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
                StructuredBufferDesc{"Transforms",         Diligent::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,        memorySettings.maxRenderers / 2},
                StructuredBufferDesc{"StaticInstances",    Diligent::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,        memorySettings.maxRenderers / 2},
                StructuredBufferDesc{"SkinnedInstances",   Diligent::SHADER_TYPE_VERTEX, memorySettings.maxRenderers,        memorySettings.maxRenderers / 2},
                StructuredBufferDesc{"Lights",             Diligent::SHADER_TYPE_VS_PS,  GetTotalLightCount(memorySettings), GetTotalLightCount(memorySettings)},
                StructuredBufferDesc{"Materials",          Diligent::SHADER_TYPE_PIXEL,  memorySettings.maxRenderers,        memorySettings.maxRenderers / 2},
                StructuredBufferDesc{"Bones",              Diligent::SHADER_TYPE_VERTEX, memorySettings.maxBones,            memorySettings.maxBones / 4},
                StructuredBufferDesc{"Particles",          Diligent::SHADER_TYPE_VS_PS,  memorySettings.maxParticles,        memorySettings.maxParticles / 4},
                TextureBufferDesc{"Textures",              Diligent::SHADER_TYPE_PIXEL,  memorySettings.maxTextures},
                UniformBufferDesc{"EnvironmentProperties", Diligent::SHADER_TYPE_VS_PS},
                UniformBufferDesc{"WireframeProperties",   Diligent::SHADER_TYPE_VS_PS}
              },
              m_perPassSignature{
                device, context,
                "PerPassResourceSignature",
                1,
                SinkBufferDesc{"ColorSinks",               Diligent::SHADER_TYPE_PIXEL, 20},
                SinkBufferDesc{"DepthSinks",               Diligent::SHADER_TYPE_PIXEL, 20},
                SinkBufferDesc{"PostProcessSinks",         Diligent::SHADER_TYPE_PIXEL, 3, true},
                SinkBufferDesc{"UniShadowMapSinks",        Diligent::SHADER_TYPE_VS_PS, memorySettings.maxDirectionalLights + memorySettings.maxSpotLights},
                CubeSinkBufferDesc{"PointShadowMapSinks",  Diligent::SHADER_TYPE_VS_PS, 1},
                UniformBufferDesc{"PostProcessProperties", Diligent::SHADER_TYPE_PIXEL, true},
                UniformBufferDesc{"SinkIndices",           Diligent::SHADER_TYPE_VS_PS}
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
