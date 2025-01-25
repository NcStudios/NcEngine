#pragma once

#include "ncengine/asset/AssetData.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include "ncasset/Assets.h"

#include <span>
#include <string>

namespace nc::graphics
{
struct StructuredBufferResourceDesc
{
    std::string resourceKey = "UninitializedStructuredBufferResourceDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
    uint32_t initialElementCount = 1u;
};

struct TextureBufferResourceDesc
{
    std::string resourceKey = "UninitializedTextureBufferResourceDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
};

struct UniformBufferResourceDesc
{
    std::string resourceKey = "UninitializedUniformBufferResourceDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
};

auto ToPipelineResourceDesc(const UniformBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const TextureBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const StructuredBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;

auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&;

template<class ResourceDesc>
auto GetVariable(const ResourceDesc& desc, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&
{
    return GetVariable(desc.shaderType, desc.resourceKey.data(), srb);
}

auto ToTextureFormat(nc::asset::asset_flags_type flags) -> Diligent::TEXTURE_FORMAT;
auto ToTextureDesc(const nc::asset::Texture& texture, Diligent::TEXTURE_FORMAT format) -> Diligent::TextureDesc;
auto ToTextureSubResData(const nc::asset::Texture& texture) -> Diligent::TextureSubResData;
void SetArrayRegion(Diligent::IShaderResourceVariable* variable, std::span<Diligent::IDeviceObject*> views, size_t offset, size_t count);
void InitializeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition = true);
} // namespace nc::graphics7
