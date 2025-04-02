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

struct CubeMapBufferDesc
{
    std::string resourceKey = "UninitializedCubeMapBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    uint32_t maxElementCount = 1u;
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

struct CubeSinkBufferResourceDesc
{
    std::string name = "";
    Diligent::TEXTURE_VIEW_TYPE viewType = Diligent::TEXTURE_VIEW_UNDEFINED;
    Diligent::TEXTURE_FORMAT format = Diligent::TEX_FORMAT_UNKNOWN;
    Diligent::BIND_FLAGS bindFlags = Diligent::BIND_NONE;
    Diligent::OptimizedClearValue clearValue = Diligent::OptimizedClearValue{};
    uint32_t maxTextures = 0;
};

struct UniformBufferDesc
{
    std::string resourceKey = "UninitializedUniformBufferDesc";
    Diligent::SHADER_TYPE shaderType = Diligent::SHADER_TYPE_UNKNOWN;
    bool dynamic = false;
};

auto ToPipelineResourceDesc(const UniformBufferDesc& resourceDesc)    -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const TextureBufferDesc& resourceDesc)    -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const CubeMapBufferDesc& resourceDesc)    -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const CubeSinkBufferDesc& resourceDesc)   -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const SinkBufferDesc& resourceDesc)       -> Diligent::PipelineResourceDesc;
auto ToPipelineResourceDesc(const StructuredBufferDesc& resourceDesc) -> Diligent::PipelineResourceDesc;

auto GetVariable(Diligent::SHADER_TYPE shaderType, const char* name, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&;

template<class ResourceDesc>
auto GetVariable(const ResourceDesc& desc, Diligent::IShaderResourceBinding* srb) -> Diligent::IShaderResourceVariable&
{
    return GetVariable(desc.shaderType, desc.resourceKey.data(), srb);
}

auto ToTextureFormat(nc::asset::TextureFormat format) -> Diligent::TEXTURE_FORMAT;
auto ToTextureDesc(const nc::asset::Texture& texture) -> Diligent::TextureDesc;
auto ToTextureCubeDesc(const nc::asset::CubeMap& desc) -> Diligent::TextureDesc;
auto ToTextureCubeDesc(const nc::graphics::CubeSinkBufferResourceDesc& desc, uint32_t width, uint32_t height) -> Diligent::TextureDesc;
auto ToTextureSubResData(const nc::asset::Texture& texture) -> std::vector<Diligent::TextureSubResData>;
void SetArrayRegion(Diligent::IShaderResourceVariable* variable, std::span<Diligent::IDeviceObject*> views, size_t offset, size_t count);
void InitializeCubeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition);
void InitializeArray(Diligent::IDeviceContext& context, Diligent::IRenderDevice& device, Diligent::IShaderResourceVariable* variable, uint32_t arraySize, bool transition = true);
} // namespace nc::graphics7
