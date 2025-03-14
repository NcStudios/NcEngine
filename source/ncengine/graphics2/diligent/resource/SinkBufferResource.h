#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include <vector>

namespace nc::graphics
{
struct SinkBufferResourceDesc
{
    std::string name = "";
    Diligent::TEXTURE_VIEW_TYPE viewType = Diligent::TEXTURE_VIEW_UNDEFINED;
    Diligent::TEXTURE_FORMAT format = Diligent::TEX_FORMAT_UNKNOWN;
    Diligent::BIND_FLAGS bindFlags = Diligent::BIND_NONE;
    Diligent::OptimizedClearValue clearValue = Diligent::OptimizedClearValue{};
    uint32_t maxTextures = 0;
};

auto MakeColorSinkBufferDesc(uint32_t maxTextures) -> SinkBufferResourceDesc;
auto MakeDepthSinkBufferDesc(uint32_t maxTextures) -> SinkBufferResourceDesc;
auto MakeShadowSinkBufferDesc(uint32_t maxTextures) -> SinkBufferResourceDesc;

class SinkBufferResource
{
    public:
        explicit SinkBufferResource(Diligent::IShaderResourceVariable& variable,
                                    SinkBufferResourceDesc desc,
                                    bool isCubeMap = false)
            : m_variable{&variable},
              m_desc{std::move(desc)},
              m_initialLoadComplete{false},
              m_isCubeMap{isCubeMap}
        {
        }

        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;
        static auto MakeShadowSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        void Add(Diligent::IRenderDevice& device,
                 Diligent::IDeviceContext& context,
                 uint32_t numRenderTargets,
                 uint32_t renderTargetWidth,
                 uint32_t renderTargetHeight,
                 uint32_t numSamples = 1u);

        void Resize(Diligent::IRenderDevice& device,
                    Diligent::IDeviceContext& context,
                    uint32_t renderTargetWidth,
                    uint32_t renderTargetHeight,
                    uint32_t numSamples = 1u);

        void Clear();

        void Update();

        auto GetMsaaRenderTargetView(uint32_t index) -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_renderTargetViewsMsaa.at(index)); }
        auto GetRenderTargetView(uint32_t index)     -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_renderTargetViews.at(index)); }

        auto GetMsaaTexture(uint32_t index)          -> Diligent::ITexture*     { return m_texturesMsaa.at(index); }
        auto GetTexture(uint32_t index)              -> Diligent::ITexture*     { return m_textures.at(index); }

        auto GetSinkCount() const                    -> uint32_t                { return static_cast<uint32_t>(m_textures.size()); }
        auto GetMsaaSinkCount() const                -> uint32_t                { return static_cast<uint32_t>(m_texturesMsaa.size()); }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;
        std::vector<Diligent::IDeviceObject*> m_renderTargetViews; // Render target
        std::vector<Diligent::IDeviceObject*> m_shaderResourceViews; // Shader resource
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_texturesMsaa;
        std::vector<Diligent::IDeviceObject*> m_renderTargetViewsMsaa; // Render target
        Diligent::IShaderResourceVariable* m_variable;
        SinkBufferResourceDesc m_desc;
        bool m_initialLoadComplete;
        bool m_isCubeMap;
};
} // namespace nc::graphics
