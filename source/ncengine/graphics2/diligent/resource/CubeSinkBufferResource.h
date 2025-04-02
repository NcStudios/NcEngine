#pragma once

#include "ResourceTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include <vector>

namespace nc::graphics
{
auto MakeCubeDepthSinkBufferDesc(uint32_t maxTextures) -> CubeSinkBufferResourceDesc;
auto MakeCubeShadowSinkBufferDesc(uint32_t maxTextures) -> CubeSinkBufferResourceDesc;

class CubeSinkBufferResource
{
    public:
        explicit CubeSinkBufferResource(Diligent::IShaderResourceVariable& variable,
                                        CubeSinkBufferResourceDesc desc)
            : m_variable{&variable},
              m_desc{std::move(desc)},
              m_initialLoadComplete{false}
        {
        }

        static auto MakeShadowSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        void Add(Diligent::IRenderDevice& device,
                 Diligent::IDeviceContext& context,
                 uint32_t numCubeMaps,
                 uint32_t renderTargetWidth,
                 uint32_t renderTargetHeight);

        void Resize(Diligent::IRenderDevice& device,
                    Diligent::IDeviceContext& context,
                    uint32_t renderTargetWidth,
                    uint32_t renderTargetHeight);

        void Clear();
        void Update();

        auto GetRenderTargetView(uint32_t index) -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_renderTargetViews.at(index).RawPtr()); }
        auto GetDepthTargetView(uint32_t index)  -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_depthRenderTargetViews.at(index).RawPtr()); }
        auto GetTexture(uint32_t index)          -> Diligent::ITexture*     { return m_cubeTextures.at(index); }
        auto GetSinkCount() const                -> uint32_t                { return static_cast<uint32_t>(m_cubeTextures.size()); }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_cubeTextures; 
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_depthTextures;
        std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceObject>> m_renderTargetViews; // Render target (one per face)
        std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceObject>> m_depthRenderTargetViews; // Depth Render target
        std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceObject>> m_shaderResourceViews; // Shader resource (one per cube map)
        Diligent::IShaderResourceVariable* m_variable;
        CubeSinkBufferResourceDesc m_desc;
        bool m_initialLoadComplete;
};
} // namespace nc::graphics
