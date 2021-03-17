#pragma once

namespace nc::graphics::vulkan
{
    class VulkanResource
    {
        public:
            virtual ~VulkanResource() = default;
            virtual void Bind() noexcept = 0;
    };
}