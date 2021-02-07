#pragma once

#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    static const std::vector<const char*> m_globalExtensions = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
    static const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
    #ifdef NC_DEBUG_BUILD
        static const bool m_enableValidationLayers = true;
    #else
        static const bool m_enableValidationLayers = false;
    #endif

    class Instance 
    {
        public:
            Instance(HWND hwnd, HINSTANCE hinstance);
            const vk::SurfaceKHR* GetSurface() const noexcept;
            const vk::Instance* GetInstance() const noexcept;

        private:
            vk::UniqueInstance m_instance;
            vk::UniqueSurfaceKHR m_surface;

            bool CheckValidationLayerSupport();
            void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo);
            void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo);
    };
}