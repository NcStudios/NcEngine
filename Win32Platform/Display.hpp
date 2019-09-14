#ifndef DISPLAY
#define DISPLAY

#include <iostream>
#include <windows.h>

#include <vector>

#include "../NCE/Common/Vector.hpp"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 576;
const int TILE_MAP_WIDTH  = 16;
const int TILE_MAP_HEIGHT = 9;


//TEMP
void LoadSprite();

struct RenderBuffer
{
    //Pixels are 32-bit | Mem Order: BB GG RR XX | Little Endian: XX RR GG BB
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    int bytesPerRow;
};

struct WindowDimensions
{
    int width;
    int height;
};

struct WindowData
{
    WNDCLASS WindowClass;
    HWND Window;
    HDC DeviceContext;
};

extern RenderBuffer renderBuffer;
extern WindowDimensions windowDimensions;
extern WindowData windowData;


void InitializeRenderBuffer(RenderBuffer *t_renderBuffer, int t_width, int t_height);
void DisplayBufferInWindow(HDC t_deviceContext, RenderBuffer *t_renderBuffer);
WindowDimensions GetWindowDimensions(HWND t_hwnd);
void OnWindowResize(HWND t_hwnd);

int GetTileMapValueAtCoordinates(int t_x, int t_y, int t_tileWidth);
void RenderTileMap(RenderBuffer *t_renderBuffer);

void Render(std::vector<NCE::Common::Rect> &t_spriteRects);
void RenderGameObjects(RenderBuffer *t_renderBuffer, std::vector<NCE::Common::Rect> &t_rects);


#endif