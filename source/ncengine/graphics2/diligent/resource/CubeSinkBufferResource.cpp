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
        .name = "Depth Cubemap",
        .viewType = TEXTURE_VIEW_DEPTH_STENCIL,
        .format = OffScreenDepthRTFormat,
        .bindFlags = BIND_SHADER_RESOURCE | BIND_DEPTH_STENCIL,
        .clearValue = OptimizedClearValue{
            .Format = OffScreenDepthRTFormat,
            .DepthStencil = DepthStencilClearValue{1.0f, 0}
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
    samplerDesc.MagFilter = FILTER_TYPE::FILTER_TYPE_COMPARISON_LINEAR;
    samplerDesc.MinFilter = FILTER_TYPE::FILTER_TYPE_COMPARISON_LINEAR;
    samplerDesc.MipFilter = FILTER_TYPE::FILTER_TYPE_COMPARISON_LINEAR;
    samplerDesc.ComparisonFunc = COMPARISON_FUNC_LESS;

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

    auto numDepthViews = numCubeMaps * 6;

    if (numCubeMaps + m_cubeTextures.size() > maxTextures)
    {
        throw NcError{"Max cube map count exceeded"};
    }

    m_cubeTextures.reserve(m_cubeTextures.size() + numCubeMaps);
    m_depthRenderTargetViews.reserve(m_depthRenderTargetViews.size() + numDepthViews);
    m_shaderResourceViews.reserve(m_shaderResourceViews.size() + numCubeMaps);

    // Create depth cubemap descriptor (D32_FLOAT format)
    TextureDesc depthCubeDesc{};
    depthCubeDesc.Type = RESOURCE_DIM_TEX_CUBE;
    depthCubeDesc.Width = renderTargetWidth;
    depthCubeDesc.Height = renderTargetHeight;
    depthCubeDesc.ArraySize = 6;
    depthCubeDesc.MipLevels = 1;
    depthCubeDesc.Format = OffScreenDepthRTFormat;
    depthCubeDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_DEPTH_STENCIL;
    depthCubeDesc.ClearValue.Format = OffScreenDepthRTFormat;
    depthCubeDesc.ClearValue.DepthStencil.Depth = 1.0f;
    depthCubeDesc.ClearValue.DepthStencil.Stencil = 0;
    depthCubeDesc.SampleCount = 1;

    for (auto i = 0u; i < numCubeMaps; i++)
    {
        // Create depth cubemap texture
        const auto depthCubeName = fmt::format("DepthCube {} : {}", m_desc.name, i);
        depthCubeDesc.Name = depthCubeName.data();
        auto& depthCubeTexture = m_cubeTextures.emplace_back(MakeTexture(device, depthCubeDesc));

        // Create a depth-stencil view for each face of the cubemap
        for (auto face = 0u; face < 6; face++)
        {
            const auto faceName = fmt::format("DSV for DepthCube {}: Face {}", m_desc.name, face);
            auto depthViewDesc = TextureViewDesc{faceName.c_str(), TEXTURE_VIEW_DEPTH_STENCIL, RESOURCE_DIM_TEX_2D_ARRAY};
            depthViewDesc.MostDetailedMip = 0;
            depthViewDesc.FirstArraySlice = face;
            depthViewDesc.NumArraySlices = 1;
            RefCntAutoPtr<ITextureView> depthDsv;
            depthCubeTexture->CreateView(depthViewDesc, &depthDsv);

            NC_ASSERT(depthDsv, "Error creating the depth cubemap face DSV.");
            m_depthRenderTargetViews.push_back(std::move(depthDsv));
        }

        // Create the shader resource view for sampling the entire depth cubemap
        const auto srvName = fmt::format("SRV for DepthCube {}", m_desc.name);
        auto shaderResourceViewDesc = TextureViewDesc{srvName.c_str(), TEXTURE_VIEW_SHADER_RESOURCE, RESOURCE_DIM_TEX_CUBE};
        shaderResourceViewDesc.NumArraySlices = 6;
        RefCntAutoPtr<ITextureView> cubeMapSrv;
        depthCubeTexture->CreateView(shaderResourceViewDesc, &cubeMapSrv);
        NC_ASSERT(cubeMapSrv, "Error creating the depth cubemap SRV.");
        m_shaderResourceViews.push_back(std::move(cubeMapSrv));
    }

    std::vector<IDeviceObject*> rawPointers;
    rawPointers.reserve(m_shaderResourceViews.size());
    for (const auto& srv : m_shaderResourceViews) {
        rawPointers.push_back(srv.RawPtr());
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
