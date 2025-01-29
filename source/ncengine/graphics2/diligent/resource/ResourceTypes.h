#pragma once

#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

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

struct UniformBufferDesc
{
    std::string resourceKey = "UninitializedUniformBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
};

auto ToPipelineResourceDesc(const UniformBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const TextureBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const StructuredBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc;

auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&;

template<class ResourceDesc>
auto GetVariable(const ResourceDesc& desc, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&
{
    return GetVariable(desc.shaderType, desc.resourceKey.data(), srb);
}
} // namespace nc::graphics7
