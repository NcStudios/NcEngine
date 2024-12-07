#pragma once

#include "base/DynamicUniformBuffer.h"
#include "graphics2/ShaderTypes.h"

namespace nc::graphics
{
class WireframeBufferResource
{
    public:
        explicit WireframeBufferResource(Diligent::IDeviceContext& context,
                                         Diligent::IRenderDevice& device,
                                         Diligent::IShaderResourceVariable& variable);

        void Update(Diligent::IDeviceContext& context,
                    const WireframeData& data);

    private:
        DynamicUniformBuffer m_buffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
