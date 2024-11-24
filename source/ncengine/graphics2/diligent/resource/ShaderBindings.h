#pragma once

#include "MeshBuffer.h"
#include "PerFrameResourceSignature.h"
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
                                uint32_t initialMaterialSizeHint)
            : m_perFrameSignature{
                context, device,
                "PerFrameResourceSignature",
                0,
                StructuredBufferResourceDesc{"MeshRendererBufferData",     Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxMeshRenderers,     maxMeshRenderers},
                StructuredBufferResourceDesc{"DirectionalLightBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxDirectionalLights, maxDirectionalLights},
                StructuredBufferResourceDesc{"PointLightBufferData",       Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxPointLights,       maxPointLights},
                StructuredBufferResourceDesc{"SpotLightBufferData",        Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxSpotLights,        maxSpotLights},
                StructuredBufferResourceDesc{"MaterialBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, maxMeshRenderers,     initialMaterialSizeHint},
                TextureBufferResourceDesc{"TextureBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, maxTextures},
                UniformBufferResourceDesc{"EnvironmentBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS}
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

        auto GetMeshBuffer() -> MeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        PerFrameResourceSignature m_perFrameSignature;
        MeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
