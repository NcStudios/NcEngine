#include "../Include/RenderingSystem.h"

namespace nc::internal{

const int TILE_MAP_WIDTH  = 16;
const int TILE_MAP_HEIGHT = 9;
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


RenderingSystem::RenderingSystem(void (*displayBuffer)(void*, BITMAPINFO&, int, int), int initialWidth, int initialHeight)
{
    CopyBufferToScreen = displayBuffer;
    AllocateRenderBuffer(initialWidth, initialHeight);
}

void RenderingSystem::AllocateRenderBuffer(int width, int height)
{
    if (m_buffer.memory)
        VirtualFree(m_buffer.memory, 0, MEM_RELEASE);

    m_buffer.width  = width;
    m_buffer.height = height;
    
    m_buffer.info.bmiHeader.biSize = sizeof(m_buffer.info.bmiHeader);
    m_buffer.info.bmiHeader.biWidth = width;
    m_buffer.info.bmiHeader.biHeight = -height;
    m_buffer.info.bmiHeader.biPlanes = 1;
    m_buffer.info.bmiHeader.biBitCount = 32;
    m_buffer.info.bmiHeader.biCompression = BI_RGB;

    int const bytesPerPixel = 4;
    int bitmapMemorySize = (width * height) * bytesPerPixel;
    m_buffer.memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    m_buffer.bytesPerRow = width*bytesPerPixel;
}

void RenderingSystem::RenderTileMap()
{
    int tileWidth = 64;
    int tileValue;

    uint32_t colorOne = 11111111;
    uint32_t colorTwo = 10101010;

    uint8_t *row = (uint8_t *)m_buffer.memory;

    for (int y = 0; y < m_buffer.height; y++)
    {
        uint32_t *pixel = (uint32_t *)row;

        for (int x = 0; x < m_buffer.width; x++)
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
        row += m_buffer.bytesPerRow;
    }
}

void RenderingSystem::RenderSprites(const std::vector<Transform> &transforms)
{
    uint32_t characterColor = 01010101; //temp value until sprite loading implemented
    int minX = 0;
    int minY = 0;
    int maxX = (m_buffer.width)  - 1;
    int maxY = (m_buffer.height) - 1;

    int left, top, right, bottom,
        visibleLeft, visibleRight, visibleTop, visibleBottom,
        visibleWidth, visibleHeight;

    uint8_t  *row;
    uint32_t *pixel;

    for (auto& position : transforms)
    {
        if(!position.IsRenderingEnabled()) continue;

        Vector4 rect = position.GetRect();
        left   = rect.GetX();
        top    = rect.GetY();
        right  = left + rect.GetZ();
        bottom = top  + rect.GetW();

        visibleLeft   = left   < minX ? minX : left;
        visibleRight  = right  > maxX ? maxX : right;
        visibleTop    = top    < minY ? minY : top;
        visibleBottom = bottom > maxY ? maxY : bottom;
        visibleWidth  = visibleRight  - visibleLeft;
        visibleHeight = visibleBottom - visibleTop;

        row = (uint8_t*)m_buffer.memory;
        row += visibleTop * (m_buffer.bytesPerRow);

        for (int yPixelPos = 0; yPixelPos < visibleHeight; yPixelPos++)
        {
            pixel = (uint32_t*)row;
            pixel += visibleLeft;

            for (int xPixelPos = 0; xPixelPos < visibleWidth; xPixelPos++)
            {
                *pixel++ = characterColor;
            }

            row += m_buffer.bytesPerRow;
        }
    } //end rect loop
}

int RenderingSystem::GetTileMapValueAtCoordinates(int x, int y, int tileWidth)
{
    int tileX = x / tileWidth;
    int tileY = y / tileWidth;
    return TileMap[tileY][tileX];
}

void RenderingSystem::StartRenderCycle(const std::vector<Transform> &transforms)
{
    RenderTileMap();
    RenderSprites(transforms);
    CopyBufferToScreen(m_buffer.memory, m_buffer.info, m_buffer.width, m_buffer.height);
}

void RenderingSystem::ForceRender()
{
    CopyBufferToScreen(m_buffer.memory, m_buffer.info, m_buffer.width, m_buffer.height);
}

} //end namespace nc::internal