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
                                uint32_t initialInstanceSizeHint,
                                uint32_t initialMaterialSizeHint)
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
                StructuredBufferResourceDesc{"BoneBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX, maxMeshRenderers * 100, 1}, // todo: size is guess, should be in sync w/ cpu-side object
                TextureBufferResourceDesc{"TextureBufferData",             Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,  maxTextures},
                UniformBufferResourceDesc{"EnvironmentBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"WireframeBufferData",           Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS},
                UniformBufferResourceDesc{"OutlinePassBufferData",         Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL}
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
