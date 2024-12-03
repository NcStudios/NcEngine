#pragma once

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <string>

namespace nc::graphics
{
constexpr auto SwapChainColorRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto SwapChainDepthRTIndex = std::numeric_limits<uint32_t>::max();
constexpr auto OffScreenColorRTFormat = Diligent::TEX_FORMAT_RGBA8_UNORM;
constexpr auto OffScreenDepthRTFormat = Diligent::TEX_FORMAT_D32_FLOAT;

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
} // namespace nc::graphics7
