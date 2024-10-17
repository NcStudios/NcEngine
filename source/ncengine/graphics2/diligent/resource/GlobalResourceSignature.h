#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <memory>

namespace nc::graphics
{
class GlobalTextureBufferResource;

class GlobalResourceSignature
{
    public:
        static constexpr auto BindingIndex = 0u;
        static constexpr auto SignatureName = "GlobalTextureBuffer";
        static constexpr auto GlobalTextureBufferShaderVariableName = "g_textures";

        explicit GlobalResourceSignature(Diligent::IRenderDevice& device, uint32_t maxTextures);

        void Commit(Diligent::IDeviceContext& context)
        {
            context.CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }

        auto GetResourceSignature() -> Diligent::IPipelineResourceSignature&
        {
            return *m_signature;
        }

        auto GetGlobalTextureBuffer() -> GlobalTextureBufferResource&
        {
            return *m_textureResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<GlobalTextureBufferResource> m_textureResource;
};
} // namespace nc::graphics
