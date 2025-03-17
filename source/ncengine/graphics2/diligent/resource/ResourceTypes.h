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
struct StructuredBufferDesc
{
    std::string resourceKey = "UninitializedStructuredBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
    uint32_t initialElementCount = 1u;
};

struct TextureBufferDesc
{
    std::string resourceKey = "UninitializedTextureBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
};

struct SinkBufferDesc
{
    std::string resourceKey = "UninitializedSinkBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
    bool dynamic = false;
};

struct CubeSinkBufferDesc
{
    std::string resourceKey = "UninitializedCubeSinkBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
    bool dynamic = false;
};

struct UniformBufferDesc
{
    std::string resourceKey = "UninitializedUniformBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    bool dynamic = false;
};

auto ToPipelineResourceDesc(const UniformBufferDesc& resourceDesc)    -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const TextureBufferDesc& resourceDesc)    -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const CubeSinkBufferDesc& resourceDesc)   -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const SinkBufferDesc& resourceDesc)       -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const StructuredBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc;

auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&;

template<class ResourceDesc>
auto GetVariable(const ResourceDesc& desc, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&
{
    return GetVariable(desc.shaderType, desc.resourceKey.data(), srb);
}

auto ToTextureFormat(nc::asset::asset_flags_type flags) -> Diligent::TEXTURE_FORMAT;
auto ToTextureDesc(const nc::asset::Texture& texture, Diligent::TEXTURE_FORMAT format, uint32_t mipLevels = 1u) -> Diligent::TextureDesc;
auto ToTextureSubResData(const nc::asset::Texture& texture) -> Diligent::TextureSubResData;
void SetArrayRegion(Diligent::IShaderResourceVariable* variable, std::span<Diligent::IDeviceObject*> views, size_t offset, size_t count);
void InitializeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition = true);
} // namespace nc::graphics7
