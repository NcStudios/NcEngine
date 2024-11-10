#include "NativeWindow.h"

#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_X11 1

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace nc::graphics
{
auto GetNativeWindow(GLFWwindow* window) -> Diligent::NativeWindow
{
    auto x11Handle = glfwGetX11Window(window);
    auto x11Display = glfwGetX11Display();
    NC_ASSERT(x11Handle, "Error getting the X11 window handle.");
    NC_ASSERT(x11Display, "Error getting the X11 display from the GLFW window.");
    return Diligent::LinuxNativeWindow{
        static_cast<uint32_t>(x11Window),
        x11Display,
        nullptr
    };
}
} // namespace nc::graphics
