#include "Graphics2.h"

namespace nc::graphics
{
    Graphics2::Graphics2()
    {
        InitializeVulkan();
    }

    void Graphics2::InitializeVulkan()
    {
        vk::ApplicationInfo applicationInfo( "NCEngine", 1, "Vulkan.hpp", 1, VK_API_VERSION_1_2 );
        vk::InstanceCreateInfo instanceCreateInfo ( {}, &applicationInfo );

        try
        {
            m_vulkan = vk::createInstanceUnique(instanceCreateInfo);
        }
        catch (std::exception& error)
        {
            std::string message = "InitializeVulkan - Error: ";
            message.append(error.what());
            throw std::runtime_error(message);
        }
    }
}