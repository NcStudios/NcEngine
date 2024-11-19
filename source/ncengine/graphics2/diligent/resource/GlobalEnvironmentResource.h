#pragma once

#include "graphics2/ShaderTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct CameraRenderState;
struct LightRenderState;

class GlobalEnvironmentResource
{
    public:
        static constexpr auto UniformBufferName = "EnvironmentDataUniformBuffer";

        explicit GlobalEnvironmentResource(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IShaderResourceVariable& variable);

        void Update(Diligent::IDeviceContext& context, const CameraRenderState& cameraState, const LightRenderState& lightRenderState);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_uniformBuffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
