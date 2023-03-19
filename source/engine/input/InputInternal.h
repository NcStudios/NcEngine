#pragma once

#include "input/Input.h"
#include "platform/win32/NcWin32.h"
#include "GLFW/glfw3.h"

namespace nc::input
{
    struct InputItem
    {
        ButtonCode_t buttonCode;
        int action;

        InputItem(ButtonCode_t bCode, int bAction) : buttonCode(bCode), action(bAction) {}
    };

    void UpdateMousePosition(int mouseX, int mouseY);
    void SetMouseWheel(int xOffset, int yOffset);
    void SetWindow(GLFWwindow* window);
    void AddKeyToQueue(ButtonCode_t buttonCode, int action);
    void AddMouseButtonDownToQueue(ButtonCode_t buttonCode, int action);
    void AddMouseButtonUpToQueue(ButtonCode_t buttonCode, int action);
    void ResetMouseState();
    void Flush();
}