#ifndef WIN32_PROCESS
#define WIN32_PROCESS

#include <windows.h>

struct Win32Process
{
    void(*CopyBufferToScreen)(void*, BITMAPINFO&, int, int); //copy backbuffer to screen
    void(*ProcessSystemQueue)(); //process windows messages
};


#endif