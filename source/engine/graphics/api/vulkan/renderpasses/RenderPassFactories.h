#pragma once
#include "graphics/api/vulkan/renderpasses/RenderPass.h"
#include "ncengine/asset/AssetData.h"

namespace nc::graphics::vulkan
{

class Device;
class GpuAllocator;
class RenderPass;
class ShaderBindingManager;
class Swapchain;

constexpr auto ShadowMapDimensions = nc::Vector2(1000, 1000);

auto CreateShadowMappingPassOmni(const Device* device,
                                 GpuAllocator* allocator,
                                 ShaderBindingManager* shaderBindingManager,
                                 std::vector<vk::ImageView> sourceViews) -> RenderPass;

auto CreateShadowMappingPass(const Device* device,
                             GpuAllocator* allocator,
                             ShaderBindingManager* shaderBindingManager) -> RenderPass;

auto CreateLitPass(const Device* device,
                   GpuAllocator* allocator,
                   Swapchain* swapchain,
                   const nc::Vector2& dimensions) -> RenderPass;

auto CreateDummyShadowMaps(size_t count) -> std::vector<nc::asset::CubeMapWithId>;
} // namespace nc::graphics::vulkan