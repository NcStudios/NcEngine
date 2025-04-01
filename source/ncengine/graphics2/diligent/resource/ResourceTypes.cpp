#include "ResourceTypes.h"

#include "ncutility/NcError.h"

#include "TextureLoader.h"

namespace
{
/* The PipelineResourceDesc needs to specify all used shader stages, but when we get the variable by name you must specify only one of the stages. */
auto ToCommonShaderType(Diligent::SHADER_TYPE shaderType) -> Diligent::SHADER_TYPE
{
    switch (shaderType)
    {
        case Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX:
            return shaderType;
        case Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL:
            return shaderType;
        case Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS:
            return Diligent::SHADER_TYPE::SHADER_TYPE_VERTEX;
        default:
            return Diligent::SHADER_TYPE::SHADER_TYPE_UNKNOWN;
    }
}
}

namespace nc::graphics
{
auto ToPipelineResourceDesc(const UniformBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc
    {
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
        resourceDesc.dynamic ? Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC : Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
    };
}

auto ToPipelineResourceDesc(const CubeSinkBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        resourceDesc.maxElementCount,
        Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
        resourceDesc.dynamic ? Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC : Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY
    };
}

auto ToPipelineResourceDesc(const SinkBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        resourceDesc.maxElementCount,
        Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
        resourceDesc.dynamic ? Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC : Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY
    };
}

auto ToPipelineResourceDesc(const StructuredBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc
    {
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        1,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS
    };
}

auto ToPipelineResourceDesc(const CubeMapBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        resourceDesc.maxElementCount,
        Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY
    };
}

auto ToPipelineResourceDesc(const TextureBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        resourceDesc.shaderType,
        resourceDesc.resourceKey.data(),
        resourceDesc.maxElementCount,
        Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY
    };
}

auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&
{
    auto var = srb->GetVariableByName(ToCommonShaderType(shaderType), name);
    if (!var)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", name)};
    }

    return *var;
}

auto ToTextureFormat(nc::asset::AssetSubtype subtype) -> Diligent::TEXTURE_FORMAT
{
    return subtype == nc::asset::AssetSubtype::NormalTexture
        ? Diligent::TEX_FORMAT_RGBA8_UNORM
        : Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
}

auto ToTextureCubeDesc(const nc::asset::CubeMap& desc) -> Diligent::TextureDesc
{
    Diligent::TextureDesc textureDesc{};
    textureDesc.Type = Diligent::RESOURCE_DIM_TEX_CUBE;
    textureDesc.ArraySize = 6; // ArrayLayers
    textureDesc.Width = desc.faceSideLength;
    textureDesc.Height = desc.faceSideLength;
    textureDesc.MipLevels = 1;
    textureDesc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
    textureDesc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
    textureDesc.SampleCount = 1;
    return textureDesc;
}

auto ToTextureCubeDesc(const nc::graphics::CubeSinkBufferResourceDesc& desc,
    uint32_t width,
    uint32_t height) -> Diligent::TextureDesc
{
    Diligent::TextureDesc textureDesc{};
    textureDesc.Type = Diligent::RESOURCE_DIM_TEX_CUBE;
    textureDesc.ArraySize = 6; // ArrayLayers
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.Format = desc.format;
    textureDesc.BindFlags = desc.bindFlags;
    textureDesc.ClearValue = desc.clearValue;
    textureDesc.SampleCount = 1;
    return textureDesc;
}

auto ToTextureDesc(const nc::asset::Texture& texture, Diligent::TEXTURE_FORMAT format) -> Diligent::TextureDesc
{
    auto texDesc = Diligent::TextureDesc{
        "",
        Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D,
        texture.width,
        texture.height,
        1,
        format
    };

    texDesc.MipLevels = static_cast<uint32_t>(texture.mipmaps.size() + 1);
    texDesc.BindFlags = Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;
    return texDesc;
}

auto ToTextureSubResData(const nc::asset::Texture& texture) -> std::vector<Diligent::TextureSubResData>
{
    const auto mipLevels = static_cast<uint32_t>(1 + texture.mipmaps.size());
    auto subResources = std::vector<Diligent::TextureSubResData>{};
    subResources.reserve(mipLevels);
    subResources.emplace_back(texture.pixelData.data(), texture.width * asset::Texture::numChannels);
    for (const auto& subResource : texture.mipmaps)
    {
        subResources.emplace_back(subResource.pixelData.data(), subResource.width * asset::Texture::numChannels);
    }

    return subResources;
}

void SetArrayRegion(Diligent::IShaderResourceVariable* variable, std::span<Diligent::IDeviceObject*> views, size_t offset, size_t count)
{
    variable->SetArray(
        views.data(),
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}

void InitializeCubeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition)
{
    auto dummyCubeMap = asset::CubeMapWithId
    {
        asset::CubeMap
        {
            .faceSideLength = 1u,
            .pixelData = std::vector<unsigned char> {0x1A, 0x2A, 0x3A, 0x4A}
        },
        1
    };

    auto barriers = std::vector<Diligent::StateTransitionDesc>();
    barriers.reserve(arraySize);

    auto dummyCubeMaps = std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>>();
    auto dummyViews = std::vector<Diligent::IDeviceObject*>();

    dummyCubeMaps.reserve(arraySize);
    dummyViews.reserve(arraySize);

    auto tempDummyVec = std::vector<asset::CubeMapWithId>(arraySize, dummyCubeMap);

    for (const auto& [cubeMap, id] : tempDummyVec)
    {
        auto faceSubResources = std::vector<Diligent::TextureSubResData>{6};
        for (auto& face : faceSubResources)
        {
            face = Diligent::TextureSubResData{cubeMap.pixelData.data(), cubeMap.faceSideLength * 4u};
        }
        auto texData = Diligent::TextureData{faceSubResources.data(), 6, &context};
        auto desc = ToTextureCubeDesc(cubeMap);
        auto& cubeMapTextureHandle = dummyCubeMaps.emplace_back();
        device.CreateTexture(desc, &texData, &cubeMapTextureHandle);

        if (!cubeMapTextureHandle)
        {
            throw NcError("Failed to create texture");
        }

        dummyViews.push_back(cubeMapTextureHandle->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
        barriers.emplace_back(
            cubeMapTextureHandle.RawPtr(),
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_SHADER_RESOURCE,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        );
    }

    if (transition)
    {
        context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
    }
    SetArrayRegion(variable, std::span<Diligent::IDeviceObject*>(dummyViews), 0u, arraySize);
}

void InitializeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition)
{
    const auto dummySubType = asset::AssetSubtype::None;
    const auto dummyTexture = asset::Texture{
        .width = 1u,
        .height = 1u,
        .pixelData = std::vector<unsigned char>{0x1A, 0x2A, 0x3A, 0x4A}
    };

    auto subResource = Diligent::TextureSubResData{dummyTexture.pixelData.data(), dummyTexture.width * asset::Texture::numChannels};
    const auto texData = Diligent::TextureData{&subResource, 1, &context};
    const auto desc = ToTextureDesc(dummyTexture, ToTextureFormat(dummySubType));

    auto textureHandle = Diligent::RefCntAutoPtr<Diligent::ITexture>{};
    device.CreateTexture(desc, &texData, &textureHandle);
    if (!textureHandle)
    {
        throw NcError("Failed to create texture");
    }

    if (transition)
    {
        const auto barrier = Diligent::StateTransitionDesc{
            textureHandle.RawPtr(),
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_SHADER_RESOURCE,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        };

        const auto barriers = std::vector<Diligent::StateTransitionDesc>(arraySize, barrier);
        context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
    }

    const auto textureView = textureHandle->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);
    auto dummyViews = std::vector<Diligent::IDeviceObject*>(arraySize, textureView);
    SetArrayRegion(variable, std::span<Diligent::IDeviceObject*>(dummyViews), 0u, arraySize);
}
} // namespace nc::graphics
