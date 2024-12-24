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

class SinkBufferResource
{
    public:
        explicit SinkBufferResource(Diligent::IShaderResourceVariable& variable,
                                    SinkBufferResourceDesc desc)
            : m_variable{&variable},
              m_desc{std::move(desc)}
        {
        }

        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        void Add(Diligent::IRenderDevice& device,
                 uint32_t numRenderTargets,
                 uint32_t renderTargetWidth,
                 uint32_t renderTargetHeight,
                 uint32_t numSamples = 1u);

        void Resize(Diligent::IRenderDevice& device,
                    uint32_t renderTargetWidth,
                    uint32_t renderTargetHeight,
                    uint32_t numSamples = 1u);

        void Clear();

        auto GetMsaaTextureView(uint32_t index) -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_textureViewsMsaa.at(index)); }
        auto GetTextureView(uint32_t index)     -> Diligent::ITextureView* { return static_cast<Diligent::ITextureView*>(m_textureViews.at(index)); }
        auto GetMsaaTexture(uint32_t index)     -> Diligent::ITexture*     { return m_texturesMsaa.at(index); }
        auto GetTexture(uint32_t index)         -> Diligent::ITexture*     { return m_textures.at(index); }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;
        std::vector<Diligent::IDeviceObject*> m_textureViews; // Render target
        std::vector<Diligent::IDeviceObject*> m_shaderResources; // Shader resource
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_texturesMsaa;
        std::vector<Diligent::IDeviceObject*> m_textureViewsMsaa; // Render target
        Diligent::IShaderResourceVariable* m_variable;
        SinkBufferResourceDesc m_desc;

        void SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& combinedViews, size_t offset, size_t count);
};
} // namespace nc::graphics
