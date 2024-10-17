#pragma once

#include "ShaderFactory.h"

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
                     Diligent::IPipelineResourceSignature& textureResourceSignature);

        void Render(Diligent::IDeviceContext& context);

        void CreatePipelineState(Diligent::IRenderDevice& device,
                                 Diligent::ISwapChain& swapChain,
                                 ShaderFactory& shaderFactory,
                                 Diligent::IPipelineResourceSignature& textureResourceSignature);

        void CreateGeometryBuffers(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device);
        void CreateInstanceBuffer(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device);
        void PopulateInstanceBuffer(Diligent::IDeviceContext& context);

        struct ObjectGeometry
        {
            uint32_t FirstIndex = 0;
            uint32_t NumIndices = 0;
            uint32_t BaseVertex = 0;
        };

    private:
        std::vector<ObjectGeometry> m_Geometries;

        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pBindlessPSO;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_VertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_IndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_InstanceBuffer;

        struct InstanceData
        {
            DirectX::XMMATRIX Matrix;
            uint32_t TextureInd = 0;
        };

        std::vector<InstanceData> m_InstanceData;
        std::vector<uint32_t>     m_GeometryType;

        int m_GridSize = 5;

        static constexpr int MaxGridSize  = 32;
        static constexpr int MaxInstances = MaxGridSize * MaxGridSize * MaxGridSize;
};
} // namespace nc::graphics
