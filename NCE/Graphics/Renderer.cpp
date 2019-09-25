#include "Renderer.hpp"

//Placeholder until tilemap system is implemented
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

namespace NCE::Graphics
{
    Renderer::Renderer(Win32DisplayBufferFunc displayBuffer_, int initialWidth_, int initialHeight_) : CopyBufferToScreen(displayBuffer_)
    {
        std::cout << "start Renderer()" << '\n';
        AllocateRenderBuffer(initialWidth_, initialHeight_);
        std::cout << "end Renderer()" << '\n';
    }

    void Renderer::AllocateRenderBuffer(int width_, int height_)
    {
        if (m_buffer.memory){
            VirtualFree(m_buffer.memory, 0, MEM_RELEASE);
        }

        m_buffer.width  = width_;
        m_buffer.height = height_;
        
        m_buffer.info.bmiHeader.biSize = sizeof(m_buffer.info.bmiHeader);
        m_buffer.info.bmiHeader.biWidth = width_;
        m_buffer.info.bmiHeader.biHeight = -height_;
        m_buffer.info.bmiHeader.biPlanes = 1;
        m_buffer.info.bmiHeader.biBitCount = 32;
        m_buffer.info.bmiHeader.biCompression = BI_RGB;

        int const bytesPerPixel = 4;
        int bitmapMemorySize = (width_ * height_) * bytesPerPixel;
        m_buffer.memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        m_buffer.bytesPerRow = width_*bytesPerPixel; 
    }

    void Renderer::StartRenderCycle(std::vector<NCE::Common::Rect> &spriteRects_)
    {
        RenderTileMap();
        RenderSprites(spriteRects_);
        CopyBufferToScreen(m_buffer.memory, m_buffer.info, m_buffer.width, m_buffer.height);
    }

    void Renderer::ForceRender()
    {
        //blit to screen
        CopyBufferToScreen(m_buffer.memory, m_buffer.info, m_buffer.width, m_buffer.height);
    }

    void Renderer::RenderTileMap()
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

    void Renderer::RenderSprites(std::vector<NCE::Common::Rect> &positions_)
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

        for (auto& position : positions_)
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

    int Renderer::GetTileMapValueAtCoordinates(int x_, int y_, int tileWidth_)
    {
        int tileX = x_ / tileWidth_;
        int tileY = y_ / tileWidth_;
        return TileMap[tileY][tileX];
    }
}