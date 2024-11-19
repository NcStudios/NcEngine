#pragma once

#include "GlobalMeshBuffer.h"
#include "component/ComponentResourceSignature.h"
#include "GlobalResourceSignature.h"
#include "base/StructuredBuffer.h"
#include "MaterialResourceSignature.h"

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
                                uint32_t initialMaterialSizeHint)
            : m_globalSignature{
                context, device,
                "GlobalResourceSignature",
                0,
                TextureBufferResourceDesc{"TextureBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, maxTextures},
                UniformBufferResourceDesc{"EnvironmentBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX}
              },
              m_componentSignature{
                context, device,
                "ComponentResourceSignature",
                1,
                // StructuredBufferResourceDesc{"MeshRendererBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxMeshRenderers, maxMeshRenderers}
                StructuredBufferResourceDesc{"TransformBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxMeshRenderers, maxMeshRenderers}, // don't know about max here...
                StructuredBufferResourceDesc{"InstanceBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS, maxMeshRenderers, maxMeshRenderers}

              },
              m_materialSignature{
                context, device,
                "MaterialResourceSignature",
                2,
                StructuredBufferResourceDesc{"MaterialBufferData", Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL, maxMeshRenderers, initialMaterialSizeHint}}
        {
        }

        void Update(Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device,
                    const FrontendRenderState& renderState);

        auto GetComponentSignature() -> ComponentResourceSignature&
        {
            return m_componentSignature;
        }

        auto GetGlobalSignature() -> GlobalResourceSignature&
        {
            return m_globalSignature;
        }

        auto GetMaterialSignature() -> MaterialResourceSignature&
        {
            return m_materialSignature;
        }

        auto GetMeshBuffer() -> GlobalMeshBuffer&
        {
            return m_meshBuffer;
        }

    private:
        GlobalResourceSignature m_globalSignature;
        ComponentResourceSignature m_componentSignature;
        MaterialResourceSignature m_materialSignature;
        GlobalMeshBuffer m_meshBuffer;
};
} // namespace nc::graphics
