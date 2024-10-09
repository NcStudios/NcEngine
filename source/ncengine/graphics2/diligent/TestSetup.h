#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Common/interface/BasicMath.hpp"

#include <vector>

namespace nc::graphics
{
class Camera;

class TestSetup
{
    public:
        TestSetup(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device, Diligent::ISwapChain* swapChain);

        auto GetBindlessSRB() { return m_BindlessSRB; }

        void Render(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device, Diligent::ISwapChain* swapChain, Camera* camera);

        void CreatePipelineState(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device, Diligent::ISwapChain* swapChain);
        void CreateGeometryBuffers(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device);
        void CreateInstanceBuffer(Diligent::IDeviceContext* context, Diligent::IRenderDevice* device);
        void PopulateInstanceBuffer(Diligent::IDeviceContext* context);

        struct ObjectGeometry
        {
            Diligent::Uint32 FirstIndex = 0;
            Diligent::Uint32 NumIndices = 0;
            Diligent::Uint32 BaseVertex = 0;
        };

    private:
        static constexpr int        NumTextures = 8;
        std::vector<ObjectGeometry> m_Geometries;

        bool m_BindlessMode = false;

        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pPSO;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pBindlessPSO;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_VertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_IndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_InstanceBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_VSConstants;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_SRB[NumTextures];
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_BindlessSRB;

        struct InstanceData
        {
            Diligent::float4x4 Matrix;
            Diligent::uint     TextureInd = 0;
        };

        std::vector<InstanceData> m_InstanceData;
        std::vector<Diligent::Uint32>       m_GeometryType;

        // Pixel shader output needs to be manually converted to gamma space
        bool m_ConvertPSOutputToGamma = false;

        int m_GridSize = 5;

        static constexpr int MaxGridSize  = 32;
        static constexpr int MaxInstances = MaxGridSize * MaxGridSize * MaxGridSize;
};
}
