#pragma once

#include "input/Input.h"

struct GLFWwindow;

namespace nc::input
{
    struct InputItem
    {
        KeyCode_t keyCode;
        int action;

        InputItem(KeyCode_t kCode, int kAction) : keyCode(kCode), action(kAction) {}
    };

    void UpdateMousePosition(int mouseX, int mouseY);
    void SetMouseWheel(int yOffset);
    void SetWindow(GLFWwindow* window);
    void AddKeyToQueue(KeyCode_t keyCode, int action);
    void ResetMouseState();
    void Flush();
}