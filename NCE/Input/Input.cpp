#include "Input.hpp"

namespace NCE::Input
{
    std::vector<InputItem> inputQueue;

    void AddToQueue(VKCode t_vkCode, LPARAM t_lparam)
    {
        InputItem newItem(t_vkCode, t_lparam);
        inputQueue.push_back(newItem);
    }

    void FlushQueue()
    {
        inputQueue.clear();
    }

    bool GetKeyDown(VKCode t_keyCode)
    {
        for(auto item : inputQueue)
        {
            if (item.keyCode == t_keyCode)
            {
                return true;
            }
        }

        return false;
    }
}