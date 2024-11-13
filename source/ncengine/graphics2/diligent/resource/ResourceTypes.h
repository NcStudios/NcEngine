#pragma once

#include "Graphics\GraphicsEngine\interface\GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <string>

namespace nc::graphics
{
struct StructuredBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
    uint32_t maxElementCount;
    uint32_t initialElementCount;
};

struct TextureBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
    uint32_t maxElementCount;
};

struct UniformBufferResourceDesc
{
    std::string resourceKey;
    Diligent::SHADER_TYPE shaderType;
};

auto ToPipelineResourceDesc(const UniformBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const TextureBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const StructuredBufferResourceDesc& resourceDesc) -> Diligent::PipelineResourceDesc;

/* The PipelineResourceDesc needs to specify all used shader stages, but when we get the variable by name you must specify only one of the stages. */
auto ToCommonShaderType(Diligent::SHADER_TYPE shaderType) -> Diligent::SHADER_TYPE;

} // namespace nc::graphics7
