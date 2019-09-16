#include "Input.hpp"

namespace NCE::Input
{
    std::vector<InputItem> downKeys, upKeys;
    uint32_t MouseX, MouseY;

    void AddToQueue(VKCode t_vkCode, LPARAM t_lparam)
    {
        //how often is this called unnecessarily?

        bool WasDown = ((t_lparam & (1 << 30)) != 0);
        bool IsDown  = ((t_lparam & (1 << 31)) == 0);
        
        if (WasDown)
        {
            if (!IsDown)
            {
                InputItem newItem(t_vkCode, t_lparam);
                upKeys.push_back(newItem);
            }
        }
        else if (IsDown)
        {
            InputItem newItem(t_vkCode, t_lparam);
            downKeys.push_back(newItem);
        }
    }

    void FlushQueue()
    {
        downKeys.clear();
        upKeys.clear();
    }

    void UpdateMousePosition(LPARAM t_lparam)
    {
        MouseX = GET_X_LPARAM(t_lparam); // extracted values can be negative so HI/LO WORD doesn't work
        MouseY = GET_Y_LPARAM(t_lparam);
    }



    bool GetKeyDown(VKCode t_keyCode)
    {
        for(auto item : downKeys)
        {
            if (item.keyCode == t_keyCode)
            {
                return true;
            }
        }

        return false;
    }

    bool GetKeyUp(VKCode t_keyCode)
    {
        for(auto item : upKeys)
        {
            if (item.keyCode == t_keyCode)
            {
                return true;
            }
        }

        return false;
    }

    bool GetKey(VKCode t_keyCode)
    {
        //if sig bit is 1, key is down
        return (GetAsyncKeyState(t_keyCode) & (1 << 15)) == 0 ? false : true;
    }
}