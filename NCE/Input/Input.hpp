#ifndef NCE_INPUT
#define NCE_INPUT

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <iostream>

namespace NCE::Input
{
    typedef uint32_t VKCode;

    struct InputItem
    {
        VKCode keyCode;
        LPARAM lparam;

        InputItem(VKCode vkCode_, LPARAM lparam_)
        {
            keyCode = vkCode_;
            lparam  = lparam_;
        }
    };

    extern std::vector<InputItem> downKeys, upKeys;
    extern uint32_t MouseX, MouseY;
    void UpdateMousePosition(LPARAM lparam_);

    void AddToQueue(VKCode vkCode_, LPARAM lparam_);
    void Flush();

    bool GetKeyDown(VKCode keyCode_); //true when key is first pressed
    bool GetKeyUp(VKCode keyCode_);   //true when key is released
    bool GetKey(VKCode keyCode_);     //true while key is held down

}



#endif