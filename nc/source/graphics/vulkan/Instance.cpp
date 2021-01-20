#include "Instance.h"
//#include "glfw/glfw3.h"

namespace nc::graphics::vulkan
{
    const std::vector<const char*> Instance::m_validationLayers = { "VK_LAYER_KHRONOS_validation" };

    vk::UniqueInstance Instance::Create()
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        EnableValidationLayers(instanceCreateInfo);
        //SetGlobalExtensions(instanceCreateInfo);

        try
        {
            return vk::createInstanceUnique(instanceCreateInfo);
        }
        catch (std::exception& error)
        {
            std::string message = "Instance::Create - Error: ";
            message.append(error.what());
            throw std::runtime_error(message);
        }
    }

    // void Instance::SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo)
    // {
    //     uint32_t glfwExtensionCount = 0;
    //     const char** glfwExtensions;

    //     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //     instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    //     instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    // }

    void Instance::EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo)
    {
         if (m_enableValidationLayers)
        {
            if (!CheckValidationLayerSupport())
            {
                throw std::runtime_error("Instance::EnableValidationLayers - Validation layers requested but not available.");
            }

            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();
        }
        else 
        {
            instanceCreateInfo.enabledLayerCount = 0;
        }
    }

    bool Instance::CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        auto layerResult = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
        if (layerResult != vk::Result::eSuccess)
        {
            return false;
        }

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        auto layerDataResult = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        if (layerDataResult != vk::Result::eSuccess)
        {
            return false;
        }

        for (const char* layerName : m_validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
}