#pragma once

#include "base/DynamicUniformBuffer.h"

#include "ncengine/graphics/PostProcess.h"

#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

#include <vector>

namespace nc::graphics
{
struct PostProcessState;

// Set of uniform buffer variables for post process property types. The actual buffers are owned by pass instances.
class PostProcessPropertyBufferResource
{
    public:
        explicit PostProcessPropertyBufferResource(Diligent::IDeviceContext& context,
                                                   Diligent::IRenderDevice& device,
                                                   Diligent::IShaderResourceVariable& postProcessPassVariable);

        void Update(Diligent::IDeviceContext& context,
                    const PostProcessPassProperties& properties);

    private:
        DynamicUniformBuffer m_outlinePassBuffer;
        DynamicUniformBuffer m_noisePassBuffer;
        Diligent::IShaderResourceVariable* m_postProcessPassVariable;
};
} // namespace nc::graphics
