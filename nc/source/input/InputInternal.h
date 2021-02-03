#pragma once

#include "Input.h"
#include "platform/win32/NcWin32.h"

namespace nc::input
{
    struct InputItem
    {
        KeyCode_t keyCode;
        LPARAM lparam;

        InputItem(KeyCode_t kCode, LPARAM param) : keyCode(kCode), lparam(param) {}
    };

    void UpdateMousePosition(LPARAM lParam);
    void SetMouseWheel(WPARAM wParam, LPARAM);
    void AddToQueue(KeyCode_t vkCode, LPARAM lParam);
    void ResetMouseState();
    void Flush();
}