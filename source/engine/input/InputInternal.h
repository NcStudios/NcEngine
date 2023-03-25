#pragma once

#include "input/Input.h"
#include "GLFW/glfw3.h"

namespace nc::input
{
    struct InputItem
    {
        KeyCode_t buttonCode;
        int action;

        InputItem(KeyCode_t bCode, int bAction) : buttonCode(bCode), action(bAction) {}
    };

    void UpdateMousePosition(int mouseX, int mouseY);
    void SetMouseWheel(int xOffset, int yOffset);
    void SetWindow(GLFWwindow* window);
    void AddKeyToQueue(KeyCode_t buttonCode, int action);
    void ResetMouseState();
    void Flush();
}