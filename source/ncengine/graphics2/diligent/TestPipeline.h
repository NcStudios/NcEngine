#pragma once

#include "ShaderFactory.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ToonRenderer.h"

#include "DirectXMath.h"
#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include <vector>

namespace nc::graphics
{
struct FrontendRenderState;

class TestPipeline
{
    public:
        TestPipeline(Diligent::IRenderDevice& device,
                     Diligent::ISwapChain& swapChain,
                     ShaderFactory& shaderFactory,
                     Diligent::IPipelineResourceSignature& globalResourceSignature,
                     Diligent::IPipelineResourceSignature& componentResourceSignature,
                     Diligent::IPipelineResourceSignature& materialResourceSignature);

        void Render(Diligent::IDeviceContext& context,
                    ecs::ExplicitEcs<ToonRenderer> ecs,
                    const nc::graphics::FrontendRenderState& renderState);

        void CreatePipelineState(Diligent::IRenderDevice& device,
                                 Diligent::ISwapChain& swapChain,
                                 ShaderFactory& shaderFactory,
                                 Diligent::IPipelineResourceSignature& globalResourceSignature,
                                 Diligent::IPipelineResourceSignature& componentResourceSignature,
                                 Diligent::IPipelineResourceSignature& materialResourceSignature);

        void CreateInstanceBuffer(Diligent::IRenderDevice& device);

    private:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pBindlessPSO;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_InstanceBuffer;

        struct InstanceData
        {
            uint32_t TextureInd = 0;
        };

        std::vector<InstanceData> m_InstanceData;
};
} // namespace nc::graphics
