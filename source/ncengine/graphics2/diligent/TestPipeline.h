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
class TestPipeline
{
    public:
        TestPipeline(Diligent::IDeviceContext& context,
                     Diligent::IRenderDevice& device,
                     Diligent::ISwapChain& swapChain,
                     ShaderFactory& shaderFactory,
                     Diligent::IPipelineResourceSignature& globalResourceSignature,
                     Diligent::IPipelineResourceSignature& componentResourceSignature);

        void Render(Diligent::IDeviceContext& context,
                    ecs::ExplicitEcs<ToonRenderer> ecs);

        void CreatePipelineState(Diligent::IRenderDevice& device,
                                 Diligent::ISwapChain& swapChain,
                                 ShaderFactory& shaderFactory,
                                 Diligent::IPipelineResourceSignature& globalResourceSignature,
                                 Diligent::IPipelineResourceSignature& componentResourceSignature);

        void CreateInstanceBuffer(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device);
        void PopulateInstanceBuffer(Diligent::IDeviceContext& context);

    private:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pBindlessPSO;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_InstanceBuffer;

        struct InstanceData
        {
            DirectX::XMFLOAT4X4 Matrix;
            uint32_t TextureInd = 0;
        };

        std::vector<InstanceData> m_InstanceData;
        std::vector<uint32_t>     m_GeometryType;

        int m_GridSize = 5;

        static constexpr int MaxGridSize  = 32;
        static constexpr int MaxInstances = MaxGridSize * MaxGridSize * MaxGridSize;
};
} // namespace nc::graphics
