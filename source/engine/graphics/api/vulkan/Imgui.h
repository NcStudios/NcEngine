#pragma once

#include "vulkan/vk_mem_alloc.hpp"

struct GLFWwindow;

namespace nc::graphics
{
class Device;
class Instance;

class Imgui
{
    public:
        explicit Imgui(const Device& device,
                       const Instance& instance,
                       GLFWwindow* window,
                       vk::RenderPass renderPass);
        ~Imgui() noexcept;

        void FrameBegin();
        void Frame();

    private:
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
    };
} // namespace nc::graphics
