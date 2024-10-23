#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include "DirectXMath.h"

namespace nc::graphics
{
struct CameraRenderState;

struct GlobalEnvironmentData
{
    DirectX::XMMATRIX cameraViewProjection;
};

class GlobalEnvironmentResource
{
    public:
        static constexpr auto UniformBufferName = "EnvironmentDataUniformBuffer";

        explicit GlobalEnvironmentResource(Diligent::IShaderResourceVariable& variable,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IDeviceContext& context);

        static auto MakeResourceDesc(std::string_view variableName) -> Diligent::PipelineResourceDesc;

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
