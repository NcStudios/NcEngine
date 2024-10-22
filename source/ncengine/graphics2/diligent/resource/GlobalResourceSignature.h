#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <memory>

namespace nc::graphics
{
class GlobalTextureBufferResource;
class GlobalEnvironmentResource;

class GlobalResourceSignature
{
    public:
        static constexpr auto BindingIndex = 0u;
        static constexpr auto SignatureName = "GlobalResourceSignature";
        static constexpr auto GlobalTextureBufferShaderVariableName = "g_textures";
        static constexpr auto GlobalEnvironmentShaderVariableName = "EnvironmentData";

        explicit GlobalResourceSignature(Diligent::IRenderDevice& device, Diligent::IDeviceContext& context, uint32_t maxTextures);
        ~GlobalResourceSignature() noexcept;

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

        auto GetGlobalEnvironment() -> GlobalEnvironmentResource&
        {
            return *m_environmentResource;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> m_signature;
        std::unique_ptr<GlobalTextureBufferResource> m_textureResource;
        std::unique_ptr<GlobalEnvironmentResource> m_environmentResource;

        auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name) -> Diligent::IShaderResourceVariable&;
};
} // namespace nc::graphics
