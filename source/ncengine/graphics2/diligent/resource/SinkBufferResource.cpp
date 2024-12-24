#include "SinkBufferResource.h"

#include "ncutility/NcError.h"

namespace
{
struct SinkTargets
{
    std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>>& textures;
    std::vector<Diligent::IDeviceObject*>& textureViews;
    std::vector<Diligent::IDeviceObject*>* shaderResources = nullptr;
};

auto MakeTextureDesc(const nc::graphics::SinkBufferResourceDesc& desc,
                     uint32_t width,
                     uint32_t height,
                     uint32_t numSamples) -> Diligent::TextureDesc
{
    Diligent::TextureDesc textureDesc{};
    textureDesc.Type = Diligent::RESOURCE_DIM_TEX_2D;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.Format = desc.format;
    textureDesc.BindFlags = desc.bindFlags;
    textureDesc.ClearValue = desc.clearValue;
    textureDesc.SampleCount = numSamples;
    return textureDesc;
}

auto MakeTexture(Diligent::IRenderDevice& device,
                 const Diligent::TextureDesc& desc) -> Diligent::RefCntAutoPtr<Diligent::ITexture>
{
    Diligent::RefCntAutoPtr<Diligent::ITexture> texture;
    device.CreateTexture(desc, nullptr, &texture);
    if (!texture)
    {
        throw nc::NcError("Failed to create texture");
    }

    return texture;
}
} // anonymous namespace

namespace nc::graphics
{
auto MakeColorSinkBufferDesc(uint32_t maxTextures) -> SinkBufferResourceDesc
{
    return SinkBufferResourceDesc{
        .name = "Color Render Target",
        .viewType = Diligent::TEXTURE_VIEW_RENDER_TARGET,
        .format = Diligent::TEX_FORMAT_RGBA8_UNORM,
        .bindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_RENDER_TARGET,
        .clearValue = Diligent::OptimizedClearValue{
            .Format = Diligent::TEX_FORMAT_RGBA8_UNORM,
            .Color = {0.0f, 0.0f, 0.0f, 0.0f},
            .DepthStencil = Diligent::DepthStencilClearValue{}
        },
        .maxTextures = maxTextures
    };
}

auto MakeDepthSinkBufferDesc(uint32_t maxTextures) -> SinkBufferResourceDesc
{
    return SinkBufferResourceDesc{
        .name = "Depth Render Target",
        .viewType = Diligent::TEXTURE_VIEW_DEPTH_STENCIL,
        .format = Diligent::TEX_FORMAT_D32_FLOAT,
        .bindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_DEPTH_STENCIL,
        .clearValue = Diligent::OptimizedClearValue{
            .Format = Diligent::TEX_FORMAT_D32_FLOAT,
            .DepthStencil = Diligent::DepthStencilClearValue{1.0f, 0}
        },
        .maxTextures = maxTextures
    };
}

auto SinkBufferResource::MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc
{
    return Diligent::ImmutableSamplerDesc{
        Diligent::SHADER_TYPE_PIXEL,
        variableName.data(),
        Diligent::SamplerDesc{}
    };
}

void SinkBufferResource::Add(Diligent::IRenderDevice& device,
                             uint32_t numRenderTargets,
                             uint32_t renderTargetWidth,
                             uint32_t renderTargetHeight,
                             uint32_t numSamples)
{
    using namespace Diligent;
    if (numRenderTargets == 0)
    {
        return;
    }

    auto targets = numSamples > 1
        ? SinkTargets{m_texturesMsaa, m_textureViewsMsaa, nullptr}
        : SinkTargets{m_textures, m_textureViews, &m_shaderResources};

    if (numRenderTargets + targets.textures.size() > m_desc.maxTextures)
    {
        throw NcError{"Max texture count exceeded"};
    }

    targets.textures.reserve(targets.textures.size() + numRenderTargets);
    targets.textureViews.reserve(targets.textureViews.size() + numRenderTargets);
    if (targets.shaderResources)
    {
        targets.shaderResources->reserve(targets.shaderResources->size() + numRenderTargets);
    }

    auto renderTargetDesc = MakeTextureDesc(m_desc, renderTargetWidth, renderTargetHeight, numSamples);
    for (auto i = 0u; i < numRenderTargets; i++)
    {
        const auto rtName = fmt::format("{}: {}", m_desc.name, i);
        renderTargetDesc.Name = rtName.data();

        auto& renderTarget = targets.textures.emplace_back(MakeTexture(device, renderTargetDesc));
        targets.textureViews.push_back(renderTarget->GetDefaultView(m_desc.viewType));
        if (targets.shaderResources)
        {
            targets.shaderResources->push_back(renderTarget->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        }
    }

    SetArrayRegion(m_shaderResources, 0u, m_shaderResources.size());
}

void SinkBufferResource::Resize(Diligent::IRenderDevice& device,
                                uint32_t renderTargetWidth,
                                uint32_t renderTargetHeight,
                                uint32_t numSamples)
{
    auto numRenderTargets = static_cast<uint32_t>(m_textures.size());
    auto numRenderTargetsMsaa = static_cast<uint32_t>(m_texturesMsaa.size());
    Clear();
    Add(device, numRenderTargets, renderTargetWidth, renderTargetHeight, 1);
    if (numSamples > 1)
    {
        Add(device, numRenderTargetsMsaa, renderTargetWidth, renderTargetHeight, numSamples);
    }
}

void SinkBufferResource::Clear()
{
    m_textures.clear();
    m_textureViews.clear();
    m_shaderResources.clear();

    m_texturesMsaa.clear();
    m_textureViewsMsaa.clear();
}

void SinkBufferResource::SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& views, size_t offset, size_t count)
{
    m_variable->SetArray(
        views.data(),
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
