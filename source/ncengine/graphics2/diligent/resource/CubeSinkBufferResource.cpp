#include "CubeSinkBufferResource.h"
#include "graphics2/diligent/pass/PassTypes.h"

#include "ncutility/NcError.h"

using namespace Diligent;

namespace
{
auto MakeTextureDesc(const nc::graphics::CubeSinkBufferResourceDesc& desc,
                     uint32_t width,
                     uint32_t height) -> TextureDesc
{
    TextureDesc textureDesc{};
    textureDesc.Type = RESOURCE_DIM_TEX_2D;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.Format = desc.format;
    textureDesc.BindFlags = desc.bindFlags;
    textureDesc.ClearValue = desc.clearValue;
    textureDesc.SampleCount = 1;
    return textureDesc;
}

auto MakeTexture(IRenderDevice& device,
                 const TextureDesc& desc) -> RefCntAutoPtr<ITexture>
{
    RefCntAutoPtr<ITexture> texture;
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
auto MakeCubeDepthSinkBufferDesc(uint32_t maxTextures) -> CubeSinkBufferResourceDesc
{
    return CubeSinkBufferResourceDesc{
        .name = "Shadow Render Target",
        .viewType = TEXTURE_VIEW_RENDER_TARGET,
        .format = OffScreenShadowMapRTFormat,
        .bindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET,
        .clearValue = OptimizedClearValue{
            .Format = OffScreenShadowMapRTFormat,
            .Color = {0.0f, 0.0f, 0.0f, 0.0f},
            .DepthStencil = DepthStencilClearValue{}
        },
        .maxTextures = maxTextures
    };
}

auto CubeSinkBufferResource::MakeShadowSamplerDesc(std::string_view variableName) -> ImmutableSamplerDesc
{
    auto samplerDesc = SamplerDesc{};
    samplerDesc.AddressU = TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MagFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    samplerDesc.MinFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;
    samplerDesc.MipFilter = FILTER_TYPE::FILTER_TYPE_LINEAR;

    return ImmutableSamplerDesc{
        SHADER_TYPE_VS_PS,
        variableName.data(),
        samplerDesc
    };
}

void CubeSinkBufferResource::Add(IRenderDevice& device,
                             IDeviceContext& ,
                             uint32_t numCubeMaps,
                             uint32_t renderTargetWidth,
                             uint32_t renderTargetHeight)
{
    using namespace Diligent;

    auto maxTextures = m_desc.maxTextures;

    if (numCubeMaps == 0)
    {
        return;
    }

    auto numRenderTargetViews = numCubeMaps * 6;

    if (numCubeMaps + m_cubeTextures.size() > maxTextures)
    {
        throw NcError{"Max cube map count exceeded"};
    }

    m_cubeTextures.reserve(m_cubeTextures.size() + numCubeMaps);
    m_renderTargetViews.reserve(m_renderTargetViews.size() + numRenderTargetViews);
    m_shaderResourceViews.reserve(m_shaderResourceViews.size() + numCubeMaps);

    auto renderTargetDesc = ToTextureCubeDesc(m_desc, renderTargetWidth, renderTargetHeight);
    auto depthTargetDesc = MakeTextureDesc(MakeCubeDepthSinkBufferDesc(m_desc.maxTextures), renderTargetWidth, renderTargetHeight);

    for (auto i = 0u; i < numCubeMaps; i++)
    {
        // Create cube texture (one for the entire cube map)
        const auto rtName = fmt::format("Cube {} : {}", m_desc.name, i);
        renderTargetDesc.Name = rtName.data();
        auto& renderTargetTexture = m_cubeTextures.emplace_back(MakeTexture(device, renderTargetDesc));

        // Create depth texture (one per face, but only need one as it'll be reused for each face)
        const auto depthRTName = fmt::format("Depth {} : {}", m_desc.name, i);
        depthTargetDesc.Name = depthRTName.data();
        auto& depthTargetTexture = m_depthTextures.emplace_back(MakeTexture(device, depthTargetDesc));

        // Create the render target view for the depth texture for the faces (one per face, but only need one as it'll be reused for each face)
        const auto depthRTVName = fmt::format("Depth RTV for CubeMap {}", m_desc.name);
        auto depthRenderTargetViewDesc = TextureViewDesc{depthRTVName.c_str(), TEXTURE_VIEW_DEPTH_STENCIL, RESOURCE_DIM_TEX_2D};
        depthRenderTargetViewDesc.MostDetailedMip = 0;
        depthRenderTargetViewDesc.NumArraySlices = 1;
        RefCntAutoPtr<ITextureView> depthRTV;
        depthTargetTexture->CreateView(depthRenderTargetViewDesc, &depthRTV);
        NC_ASSERT(depthRTV, "Error creating the depth render target view.");
        m_depthRenderTargetViews.push_back(std::move(depthRTV));

        // Create a render target view for each face of the cube map
        for (auto face = 0u; face < 6; face++)
        {
            const auto faceName = fmt::format("RTV for CubeMap {}: Face {}", m_desc.name, face);
            auto renderTargetViewDesc = TextureViewDesc{faceName.c_str(), TEXTURE_VIEW_RENDER_TARGET, RESOURCE_DIM_TEX_2D_ARRAY};
            renderTargetViewDesc.MostDetailedMip = 0;
            renderTargetViewDesc.FirstArraySlice = face;
            renderTargetViewDesc.NumArraySlices = 1;
            RefCntAutoPtr<ITextureView> cubeMapRtv;
            renderTargetTexture->CreateView(renderTargetViewDesc, &cubeMapRtv);

            NC_ASSERT(cubeMapRtv, "Error creating the cube map face render target view.");
            m_renderTargetViews.push_back(std::move(cubeMapRtv));
        }

        // Create the shader resource view for the entire cube map
        const auto cubeMapSrvName = fmt::format("SRV for CubeMap {}", m_desc.name);
        auto shaderResourceViewDesc = TextureViewDesc{cubeMapSrvName.c_str(), TEXTURE_VIEW_SHADER_RESOURCE, RESOURCE_DIM_TEX_CUBE};
        shaderResourceViewDesc.NumArraySlices = 6;
        RefCntAutoPtr<ITextureView> cubeMapSrv;
        renderTargetTexture->CreateView(shaderResourceViewDesc, &cubeMapSrv);
        NC_ASSERT(cubeMapSrv, "Error creating the cube map shader resource view.");
        m_shaderResourceViews.push_back(std::move(cubeMapSrv));
    }
    std::vector<IDeviceObject*> rawPointers;
    rawPointers.reserve(m_shaderResourceViews.size());
    for (const auto& srv : m_shaderResourceViews) {
        rawPointers.push_back(srv.RawPtr()); // Get raw pointer, RefCntAutoPtr keeps ownership
    }
    SetArrayRegion(m_variable, std::span<IDeviceObject*>(rawPointers), 0u, rawPointers.size());
}

void CubeSinkBufferResource::Resize(IRenderDevice& device,
                                IDeviceContext& context,
                                uint32_t renderTargetWidth,
                                uint32_t renderTargetHeight)
{
    auto numCubeMaps = static_cast<uint32_t>(m_cubeTextures.size());
    Clear();
    Add(device, context, numCubeMaps, renderTargetWidth, renderTargetHeight);
}

void CubeSinkBufferResource::Clear()
{
    m_cubeTextures.clear();
    m_depthTextures.clear();
    m_renderTargetViews.clear();
    m_depthRenderTargetViews.clear();
    m_shaderResourceViews.clear();
}

void CubeSinkBufferResource::Update()
{
    std::vector<IDeviceObject*> rawPointers;
    rawPointers.reserve(m_shaderResourceViews.size());
    for (const auto& srv : m_shaderResourceViews) {
        rawPointers.push_back(srv.RawPtr()); // Get raw pointer, RefCntAutoPtr keeps ownership
    }
    SetArrayRegion(m_variable, std::span<IDeviceObject*>(rawPointers), 0u, rawPointers.size());
}
} // namespace nc::graphics
