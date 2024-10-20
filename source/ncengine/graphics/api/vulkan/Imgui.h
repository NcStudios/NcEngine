#pragma once

#include "NcVulkan.h"

#include "ncengine/utility/Signal.h"

struct GLFWwindow;

namespace nc::graphics::vulkan
{
class Device;
class Instance;

class Imgui
{
    public:
        explicit Imgui(const Device& device,
                       const Instance& instance,
                       GLFWwindow* window,
                       vk::RenderPass renderPass,
                       Signal<>& onFontUpdate);
        ~Imgui() noexcept;

        void FrameBegin();
        void Frame();

    private:
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
        Connection m_buildFonts;
    };
} // namespace nc::graphics::vulkan
