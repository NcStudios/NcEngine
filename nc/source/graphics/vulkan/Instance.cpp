#include "Instance.h"

namespace nc::graphics::vulkan
{
    Instance::Instance(HWND hwnd, HINSTANCE hinstance)
    : m_instance{nullptr},
      m_surface{nullptr}
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        EnableValidationLayers(instanceCreateInfo);
        SetGlobalExtensions(instanceCreateInfo);

        try
        {
            m_instance = createInstanceUnique(instanceCreateInfo);
        }
        catch (std::exception& error)
        {
            std::string message = "Instance::Create - Error: ";
            message.append(error.what());
            throw std::runtime_error(message);
        }

        vk::SurfaceKHR surface;
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.setHinstance(hinstance);
        surfaceCreateInfo.setHwnd(hwnd);
        if (m_instance.get().createWin32SurfaceKHR(&surfaceCreateInfo, nullptr, &surface) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to get surface.");
        }

        m_surface = vk::UniqueSurfaceKHR(surface, m_instance.get());
    } 

    const vk::SurfaceKHR* Instance::GetSurface() const noexcept
    {
        return &(m_surface.get());
    }

    const vk::Instance* Instance::GetInstance() const noexcept
    {
        return &(m_instance.get());
    }

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

    void Instance::SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo)
    {
        instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_globalExtensions.size()));
        instanceCreateInfo.setPpEnabledExtensionNames(m_globalExtensions.data());
    }
}