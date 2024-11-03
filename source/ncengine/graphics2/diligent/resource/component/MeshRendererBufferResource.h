#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include "DirectXMath.h"

namespace nc::graphics
{
struct MeshRendererRenderState;

struct MeshRendererData
{
    DirectX::XMMATRIX modelMatrix;
};

class MeshRendererBufferResource
{
    public:
        static constexpr auto StructuredBufferName = "MeshRendererStructuredBuffer";

        explicit MeshRendererBufferResource(Diligent::IShaderResourceVariable& variable,
                                           Diligent::IRenderDevice& device,
                                           uint32_t maxMeshRenderers);

        static auto MakeResourceDesc(std::string_view variableName, uint32_t maxMeshRenderers) -> Diligent::PipelineResourceDesc;
        void Update(const MeshRendererRenderState renderState, Diligent::IDeviceContext& context);
        auto GetShaderVariable() -> Diligent::IShaderResourceVariable& { return *m_variable; }

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_structuredBuffer;
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxMeshRenderers;
};
} // namespace nc::graphics
