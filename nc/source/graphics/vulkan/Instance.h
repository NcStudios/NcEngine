#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics::vulkan
{
    class Instance
    {
        public:
            static vk::UniqueInstance Create();

        private: 
            static const std::vector<const char*> m_validationLayers;
            static bool CheckValidationLayerSupport();
            static void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo);
            //static void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo);

            #ifdef NC_DEBUG_BUILD
                static const bool m_enableValidationLayers = true;
            #else
                static const bool m_enableValidationLayers = false;
            #endif
    };
}