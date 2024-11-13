#pragma once

#include "graphics2/ShaderTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct CameraRenderState;

class GlobalEnvironmentResource
{
    public:
        static constexpr auto UniformBufferName = "EnvironmentDataUniformBuffer";

        explicit GlobalEnvironmentResource(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IShaderResourceVariable& variable);

        void Update(const CameraRenderState cameraState,
                    Diligent::IDeviceContext& context);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_uniformBuffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
