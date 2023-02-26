#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class Device;
class Instance;

class Imgui
{
    public:
        explicit Imgui(const Device& device);

        /** @todo #335: Move contents into constructor and remove this method during fix of imgui bug.*/
        void InitializeImgui(const Instance& instance, const Device& device, vk::RenderPass renderPass, uint32_t maxSamplesCount);

    private:
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
    };
} // namespace nc::graphics