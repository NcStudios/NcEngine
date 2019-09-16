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
    Renderer::Renderer(Win32DisplayBufferFunc t_displayBuffer, int t_initialWidth, int t_initialHeight) : _copyBufferToScreen(t_displayBuffer)
    {
        std::cout << "start Renderer()" << '\n';
        AllocateRenderBuffer(t_initialWidth, t_initialHeight);
        std::cout << "end Renderer()" << '\n';
    }

    void Renderer::AllocateRenderBuffer(int t_width, int t_height)
    {
        if (_buffer.memory){
            VirtualFree(_buffer.memory, 0, MEM_RELEASE);
        }

        _buffer.width  = t_width;
        _buffer.height = t_height;
        
        _buffer.info.bmiHeader.biSize = sizeof(_buffer.info.bmiHeader);
        _buffer.info.bmiHeader.biWidth = t_width;
        _buffer.info.bmiHeader.biHeight = -t_height;
        _buffer.info.bmiHeader.biPlanes = 1;
        _buffer.info.bmiHeader.biBitCount = 32;
        _buffer.info.bmiHeader.biCompression = BI_RGB;

        int const bytesPerPixel = 4;
        int bitmapMemorySize = (t_width * t_height) * bytesPerPixel;
        _buffer.memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        _buffer.bytesPerRow = t_width*bytesPerPixel; 
    }

    void Renderer::StartRenderCycle(std::vector<NCE::Common::Rect> &t_spriteRects)
    {
        RenderTileMap();
        RenderSprites(t_spriteRects);
        _copyBufferToScreen(_buffer.memory, _buffer.info, _buffer.width, _buffer.height);
    }

    void Renderer::ForceRender()
    {
        //blit to screen
        _copyBufferToScreen(_buffer.memory, _buffer.info, _buffer.width, _buffer.height);
    }

    void Renderer::RenderTileMap()
    {
        int tileWidth = 64;
        int tileValue;

        uint32_t colorOne = 11111111;
        uint32_t colorTwo = 10101010;

        uint8_t *row = (uint8_t *)_buffer.memory;

        for (int y = 0; y < _buffer.height; y++)
        {
            uint32_t *pixel = (uint32_t *)row;

            for (int x = 0; x < _buffer.width; x++)
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
            row += _buffer.bytesPerRow;
        }
    }

    void Renderer::RenderSprites(std::vector<NCE::Common::Rect> &t_positions)
    {
        uint32_t characterColor = 01010101;
        int minX = 0;
        int minY = 0;
        int maxX = (_buffer.width)  - 1;
        int maxY = (_buffer.height) - 1;

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

            row = (uint8_t*)_buffer.memory;
            row += visibleTop * (_buffer.bytesPerRow);

            for (int col = 0; col < visibleHeight; col++)
            {
                pixel = (uint32_t*)row;
                pixel += visibleLeft;

                for (int row = 0; row < visibleWidth; row++)
                {
                    *pixel++ = characterColor;
                }

                row += _buffer.bytesPerRow;
            }
        } //end rect loop
    }

    int Renderer::GetTileMapValueAtCoordinates(int x, int y, int tileWidth)
    {
        int tileX = x / tileWidth;
        int tileY = y / tileWidth;
        return TileMap[tileY][tileX];
    }
}