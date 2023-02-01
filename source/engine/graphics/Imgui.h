#pragma once

#include "graphics/GpuAssetsStorage.h"
#include "graphics/Initializers.h"
#include "ecs/Registry.h"

#include <functional>
#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Imgui
{
    public:
        Imgui(vk::Device device);

        void InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::RenderPass renderPass, Commands* commands, uint32_t maxSamplesCount); /** @todo: Where should Imgui stuff live? */

    private:
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
};
} // namespace nc::graphics