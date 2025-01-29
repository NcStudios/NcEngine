#include "ResourceTypes.h"

#include "ncutility/NcError.h"

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
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE
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

} // namespace nc::graphics
