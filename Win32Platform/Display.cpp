#include "Display.hpp"


WindowData windowData;
WindowDimensions windowDimensions;
RenderBuffer renderBuffer;

HBITMAP squareSprite;

void LoadSprite()
{
    squareSprite = (HBITMAP)LoadImage(NULL, TEXT("../Project/Sprites/square.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
}


int TileMap[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = 
{
    { 1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0},
    { 0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1},
    { 1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0},
    { 0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1},
    { 1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0},
    { 0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1},
    { 1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0},
    { 0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1},
    { 1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0}
};


void InitializeRenderBuffer(RenderBuffer *t_buffer, int t_width, int t_height)
{
    if (t_buffer->memory){
        VirtualFree(t_buffer->memory, 0, MEM_RELEASE);
    }

    

    t_buffer->width  = t_width;
    t_buffer->height = t_height;
    
    t_buffer->info.bmiHeader.biSize = sizeof(t_buffer->info.bmiHeader);
    t_buffer->info.bmiHeader.biWidth = t_width;
    t_buffer->info.bmiHeader.biHeight = -t_height;
    t_buffer->info.bmiHeader.biPlanes = 1;
    t_buffer->info.bmiHeader.biBitCount = 32;
    t_buffer->info.bmiHeader.biCompression = BI_RGB;

    int const bytesPerPixel = 4;
    int bitmapMemorySize = (t_width * t_height) * bytesPerPixel;
    t_buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    t_buffer->bytesPerRow = t_width*bytesPerPixel; 
}


void DisplayBufferInWindow(HDC t_deviceContext, RenderBuffer *t_buffer)
{
    int xOffset = (windowDimensions.width - t_buffer->width) / 2;
    int yOffset = (windowDimensions.height - t_buffer->height) / 2;

    StretchDIBits(t_deviceContext,                   //destination buffer (screen)
                  xOffset, yOffset,                  //destination x, y
                  t_buffer->width, t_buffer->height, //destination w, h
                  0, 0,                              //source x, y
                  t_buffer->width, t_buffer->height, //source w, h
                  t_buffer->memory,                  //source buffer         
                  &t_buffer->info,                   //buffer formatting
                  DIB_RGB_COLORS,                    //RGB data flag
                  SRCCOPY);                          //copy bits flag                             
}


WindowDimensions GetWindowDimensions(HWND t_hwnd)
{
    RECT windowRect;
    WindowDimensions dimensions;

    if (GetWindowRect(t_hwnd, &windowRect))
    {
        dimensions.width  = windowRect.right - windowRect.left;
        dimensions.height = windowRect.bottom - windowRect.top;
    }
    else
    {
        OutputDebugStr(TEXT("Failed to retrieve window rect."));
    }
    return dimensions;
}


void OnWindowResize(HWND hwnd)
{
    windowDimensions = GetWindowDimensions(hwnd);

    PatBlt(windowData.DeviceContext,
           0, 0, windowDimensions.width, windowDimensions.height,
           BLACKNESS);
}


int GetTileMapValueAtCoordinates(int x, int y, int tileWidth)
{
    int tileX = x / tileWidth;
    int tileY = y / tileWidth;
    return TileMap[tileY][tileX];
}

void RenderGameObjects(RenderBuffer *t_buffer, std::vector<NCE::Common::Rect> &t_positions)
{
    uint32_t characterColor = 01010101;
    int minX = 0;
    int minY = 0;
    int maxX = (t_buffer->width)  - 1;
    int maxY = (t_buffer->height) - 1;

    int left, top, right, bottom,
        visibleLeft, visibleRight, visibleTop, visibleBottom,
        visibleWidth, visibleHeight;

    uint8_t  *row;
    uint32_t *pixel;

    for (auto& position : t_positions)
    {
        left   = position.x;
        top    = position.y;
        right  = left + position.w;
        bottom = top  + position.h;

        visibleLeft   = left   < minX ? minX : left;
        visibleRight  = right  > maxX ? maxX : right;
        visibleTop    = top    < minY ? minY : top;
        visibleBottom = bottom > maxY ? maxY : bottom;
        visibleWidth  = visibleRight  - visibleLeft;
        visibleHeight = visibleBottom - visibleTop;

        row = (uint8_t*)t_buffer->memory;
        row += visibleTop * (t_buffer->bytesPerRow);

        for (int col = 0; col < visibleHeight; col++)
        {
            pixel = (uint32_t*)row;
            pixel += visibleLeft;

            for (int row = 0; row < visibleWidth; row++)
            {
                *pixel++ = characterColor;
            }

            row += t_buffer->bytesPerRow;
        }
    } //end rect loop
}


void RenderTileMap(RenderBuffer *t_buffer)
{
    int tileWidth = 64;
    int tileValue;

    uint32_t colorOne = 11111111;
    uint32_t colorTwo = 10101010;

    uint8_t *row = (uint8_t *)t_buffer->memory;

    for (int y = 0; y < t_buffer->height; y++)
    {
        uint32_t *pixel = (uint32_t *)row;

        for (int x = 0; x < t_buffer->width; x++)
        {
            tileValue = GetTileMapValueAtCoordinates(x, y, tileWidth);

            if (tileValue == 0)
            {
                *pixel++ = colorOne;
            }
            else
            {
                *pixel++ = colorTwo;
            }
        }
        row += t_buffer->bytesPerRow;
    }
}

// void Render(std::vector<NCE::Common::Vector4> t_spritePositions)
// {
//     RenderTileMap(&backBuffer);
//     RenderGameObjects(&backBuffer, t_spritePositions);
//     Win32DisplayBufferInWindow(WindowData.DeviceContext, &backBuffer);
// }
void Render(std::vector<NCE::Common::Rect> &t_spritePositions)
{
    RenderTileMap(&renderBuffer);
    RenderGameObjects(&renderBuffer, t_spritePositions);
    DisplayBufferInWindow(windowData.DeviceContext, &renderBuffer);
}