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
        static constexpr auto BindingIndex = 1u;
        static constexpr auto SignatureName = "ComponentResourceSignature";
        static constexpr auto MeshRendererShaderVariableName = "MeshRendererBufferData";

        explicit ComponentResourceSignature(Diligent::IRenderDevice& device, Diligent::IDeviceContext& context, uint32_t maxMeshRenderers);
        ~ComponentResourceSignature() noexcept;

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

        auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name) -> Diligent::IShaderResourceVariable&;
};
} // namespace nc::graphics
