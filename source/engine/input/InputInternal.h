#pragma once

#include "input/Input.h"
#include "platform/win32/NcWin32.h"
#include "GLFW/glfw3.h"

namespace nc::input
{
    struct InputItem
    {
        KeyCode_t keyCode;
        int action;

        InputItem(KeyCode_t kCode, int kAction) : keyCode(kCode), action(kAction) {}
    };

    void UpdateMousePosition(int mouseX, int mouseY);
    void SetMouseWheel(int xOffset, int yOffset);
    void SetWindow(GLFWwindow* window);
    void AddKeyToQueue(KeyCode_t keyCode, int action);
    void AddMouseButtonDownToQueue(KeyCode_t keyCode, int action);
    void AddMouseButtonUpToQueue(KeyCode_t keyCode, int action);
    void ResetMouseState();
    void Flush();
}