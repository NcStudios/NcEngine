#ifndef NCE_INPUT
#define NCE_INPUT

#include <windows.h>
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

    extern std::vector<InputItem> inputQueue;

    void AddToQueue(VKCode t_vkCode, LPARAM t_lparam);
    void FlushQueue();
    bool GetKeyDown(VKCode t_keyCode);
}



#endif