#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vk_mem_alloc.hpp"
#include "platform/win32/NcWin32.h"

namespace nc::graphics
{
struct Core
{
    Core(HWND hwnd, HINSTANCE hinstance);

    vk::UniqueInstance instance;
    vk::UniqueSurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice logicalDevice;
};
}