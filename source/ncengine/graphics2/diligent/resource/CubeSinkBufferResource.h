#pragma once

#include "ResourceTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include "ncutility/NcError.h"

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

        // Get depth-stencil view for a specific face (index = cubeIndex * 6 + faceIndex)
        auto GetDepthTargetView(uint32_t index) -> Diligent::ITextureView*
        {
            return static_cast<Diligent::ITextureView*>(m_depthRenderTargetViews.at(index).RawPtr());
        }
        auto GetTexture(uint32_t index)          -> Diligent::ITexture*     { return m_cubeTextures.at(index); }
        auto GetSinkCount() const                -> uint32_t                { return static_cast<uint32_t>(m_cubeTextures.size()); }
        auto GetTextureView(uint32_t index) -> void*
        {
            NC_ASSERT(index < m_shaderResourceViews.size(), "Invalid texture view index.");
            return static_cast<void*>(m_shaderResourceViews.at(index));
        }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_cubeTextures;  // Depth cubemaps
        std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceObject>> m_depthRenderTargetViews; // DSV per face (6 per cubemap)
        std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceObject>> m_shaderResourceViews; // SRV for sampling (one per cubemap)
        Diligent::IShaderResourceVariable* m_variable;
        CubeSinkBufferResourceDesc m_desc;
        bool m_initialLoadComplete;
};
} // namespace nc::graphics
