#include "NativeWindow.h"

#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_X11 1

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace nc::graphics
{
auto GetNativeWindow(GLFWwindow* window) -> Diligent::NativeWindow
{
    auto glfwWindowHandle = glfwGetX11Window(window);
    auto glfwDisplay = glfwGetX11Display();
    NC_ASSERT(glfwWindowHandle, "Error getting the X11 window handle.");
    NC_ASSERT(glfwDisplay, "Error getting the X11 display from the GLFW window.");
    return LinuxNativeWindow{
        static_cast<Diligent::Uint32>(glfwWindowHandle);
        glfwDisplay,
        nullptr
    };
}
} // namespace nc::graphics
