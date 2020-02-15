#ifndef WIN32_PROCESS_H
#define WIN32_PROCESS_H

#include <windows.h>

namespace nc::internal
{
    struct Win32Process
    {
        void(*CopyBufferToScreen)(void*, BITMAPINFO&, int, int); //copy backbuffer to screen
        void(*ProcessSystemQueue)();                             //process windows messages
    };
}


#endif