#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>

namespace nc::graphics
{
struct MaterialProperties;
struct MaterialPropertyUpdateInfo;

class MaterialPropertiesBufferResource
{
    public:
        explicit MaterialPropertiesBufferResource(Diligent::IDeviceContext& context,
                                                  Diligent::IRenderDevice& device,
                                                  Diligent::IShaderResourceVariable& variable,
                                                  uint32_t initialInstanceCountHint);

        static auto MakeResourceDesc(std::string_view variableName) -> Diligent::PipelineResourceDesc;

        void Update(const MaterialPropertyUpdateInfo& updateInfo,
                    Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

    private:
        Diligent::IShaderResourceVariable* m_variable;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_buffer;
        Diligent::IBufferView* m_view = nullptr;
        uint32_t m_bufferElementCount = 0;

        void CreateBuffer(Diligent::IDeviceContext& context,
                          Diligent::IRenderDevice& device,
                          std::span<const MaterialProperties> data);
};
} // namespace nc::graphics
