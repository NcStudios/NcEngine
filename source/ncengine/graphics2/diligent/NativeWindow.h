#pragma once

#include "Platforms/interface/NativeWindow.h"

struct GLFWwindow;

namespace nc::graphics
{
auto GetNativeWindow(GLFWwindow* window) -> Diligent::NativeWindow;
} // namespace nc::graphics
