#include "SinkBufferResource.h"
#include "ResourceTypes.h"

#include "ncutility/NcError.h"

namespace
{
struct SinkTargets
{
    std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>>& textures;
    std::vector<Diligent::IDeviceObject*>& renderTargetViews;
    std::vector<Diligent::IDeviceObject*>* shaderResourceViews = nullptr;
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
        .name = "PP Color Render Target",
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
        .name = "PP Depth Render Target",
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
                             Diligent::IDeviceContext& context,
                             uint32_t numRenderTargets,
                             uint32_t renderTargetWidth,
                             uint32_t renderTargetHeight,
                             uint32_t numSamples)
{
    using namespace Diligent;

    if (!m_initialLoadComplete)
    {
        InitializeArray(context, device, m_variable, m_desc.maxTextures, false);
        m_initialLoadComplete = true;
    }

    if (numRenderTargets == 0)
    {
        return;
    }

    auto targets = numSamples > 1
        ? SinkTargets{m_texturesMsaa, m_renderTargetViewsMsaa, nullptr}
        : SinkTargets{m_textures, m_renderTargetViews, &m_shaderResourceViews};

    if (numRenderTargets + targets.textures.size() > m_desc.maxTextures)
    {
        throw NcError{"Max texture count exceeded"};
    }

    // auto barriers = std::vector<Diligent::StateTransitionDesc>();
    // barriers.reserve(targets.textures.size() + numRenderTargets);

    targets.textures.reserve(targets.textures.size() + numRenderTargets);
    targets.renderTargetViews.reserve(targets.renderTargetViews.size() + numRenderTargets);
    if (targets.shaderResourceViews)
    {
        targets.shaderResourceViews->reserve(targets.shaderResourceViews->size() + numRenderTargets);
    }

    auto renderTargetDesc = MakeTextureDesc(m_desc, renderTargetWidth, renderTargetHeight, numSamples);
    for (auto i = 0u; i < numRenderTargets; i++)
    {
        const auto rtName = fmt::format("{}: {}", m_desc.name, i);
        renderTargetDesc.Name = rtName.data();

        auto& renderTarget = targets.textures.emplace_back(MakeTexture(device, renderTargetDesc));
        targets.renderTargetViews.push_back(renderTarget->GetDefaultView(m_desc.viewType));
        if (targets.shaderResourceViews)
        {
            targets.shaderResourceViews->push_back(renderTarget->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        }

        // barriers.emplace_back(
        //     renderTarget.RawPtr(),
        //     Diligent::RESOURCE_STATE_RENDER_TARGET,
        //     Diligent::RESOURCE_STATE_SHADER_RESOURCE,
        //     Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        // );
    }

    // context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
    SetArrayRegion(m_variable, std::span<Diligent::IDeviceObject*>(m_shaderResourceViews), 0u, m_shaderResourceViews.size());
}

void SinkBufferResource::Resize(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                uint32_t renderTargetWidth,
                                uint32_t renderTargetHeight,
                                uint32_t numSamples)
{
    auto numRenderTargets = static_cast<uint32_t>(m_textures.size());
    auto numRenderTargetsMsaa = static_cast<uint32_t>(m_texturesMsaa.size());
    Clear();
    Add(device, context, numRenderTargets, renderTargetWidth, renderTargetHeight, 1);
    if (numSamples > 1)
    {
        Add(device, context, numRenderTargetsMsaa, renderTargetWidth, renderTargetHeight, numSamples);
    }
}

void SinkBufferResource::Clear()
{
    m_textures.clear();
    m_renderTargetViews.clear();
    m_shaderResourceViews.clear();

    m_texturesMsaa.clear();
    m_renderTargetViewsMsaa.clear();
}
} // namespace nc::graphics
