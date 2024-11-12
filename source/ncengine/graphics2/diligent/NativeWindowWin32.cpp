#include "NativeWindow.h"

#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1
#ifdef GetObject
    #undef GetObject
#endif
#ifdef CreateWindow
    #undef CreateWindow
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace nc::graphics
{
auto GetNativeWindow(GLFWwindow* window) -> Diligent::NativeWindow
{
    auto glfwWindowHandle = glfwGetWin32Window(window);
    NC_ASSERT(glfwWindowHandle, "Error getting the Win32 window handle from the GLFW window.");
    return Diligent::Win32NativeWindow{glfwWindowHandle};
}
} // namespace nc::graphics
