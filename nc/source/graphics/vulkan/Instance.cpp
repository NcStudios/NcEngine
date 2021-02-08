#include "Instance.h"
#include <algorithm>

namespace
{
    const std::vector<const char*> GlobalExtensions = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
    const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    #ifdef NC_DEBUG_BUILD
        const bool IsValidationLayersEnabled = true;
    #else
        const bool IsValidationLayersEnabled = false;
    #endif

    bool CheckValidationLayerSupport();
    void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo);
    void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo);
}

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
        catch (const std::exception& error)
        {
            std::throw_with_nested(std::runtime_error("Failed to create instance."));
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
}

namespace 
{
    void EnableValidationLayers(vk::InstanceCreateInfo& instanceCreateInfo)
    {
         if (IsValidationLayersEnabled)
        {
            if (!CheckValidationLayerSupport())
            {
                throw std::runtime_error("Instance::EnableValidationLayers - Validation layers requested but not available.");
            }

            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
        }
        else 
        {
            instanceCreateInfo.enabledLayerCount = 0;
        }
    }

    bool CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        if (vk::enumerateInstanceLayerProperties(&layerCount, nullptr) != vk::Result::eSuccess)
        {
            return false;
        }

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        if (vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()) != vk::Result::eSuccess)
        {
            return false;
        }

        return std::all_of(ValidationLayers.cbegin(), ValidationLayers.cend(), [&availableLayers](const auto& requiredLayer)
        {
            return std::any_of(availableLayers.cbegin(), availableLayers.cend(), [&requiredLayer](const auto& availableLayer)
            {
                return strcmp(requiredLayer, availableLayer.layerName) == 0;
            });
        });
    }

    void SetGlobalExtensions(vk::InstanceCreateInfo& instanceCreateInfo)
    {
        instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(GlobalExtensions.size()));
        instanceCreateInfo.setPpEnabledExtensionNames(GlobalExtensions.data());
    }
}