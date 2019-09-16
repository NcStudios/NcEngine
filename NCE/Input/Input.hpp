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

        InputItem(VKCode t_vkCode, LPARAM t_lparam)
        {
            keyCode = t_vkCode;
            lparam  = t_lparam;
        }
    };


    //extern std::vector<InputItem> inputQueue; //use map?
    extern std::vector<InputItem> downKeys, upKeys;
    extern uint32_t MouseX, MouseY;
    void UpdateMousePosition(LPARAM t_lparam);

    void AddToQueue(VKCode t_vkCode, LPARAM t_lparam);
    void FlushQueue();

    bool GetKeyDown(VKCode t_keyCode); //true when key is first pressed
    bool GetKeyUp(VKCode t_keyCode);   //true when key is released
    bool GetKey(VKCode t_keyCode);     //true while key is held down

}



#endif