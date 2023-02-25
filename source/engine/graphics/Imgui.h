#pragma once

#include "ecs/Registry.h"
#include "graphics/GpuAssetsStorage.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Imgui
{
    public:
        explicit Imgui(vk::Device device);

        /* @todo: Move contents into constructor and remove this method during fix of imgui bug.*/
        void InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::RenderPass renderPass, Commands *commands, uint32_t maxSamplesCount);

    private:
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
    };
} // namespace nc::graphics