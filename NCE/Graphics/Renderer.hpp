#ifndef NCE_GRAPHICS_RENDERER
#define NCE_GRAPHICS_RENDERER

#include <windows.h>
#include <stdint.h>

#include <iostream>

#include <vector>

#include "../Common/Vector.hpp"

namespace NCE::Graphics
{
    typedef void (*Win32DisplayBufferFunc)(void*, BITMAPINFO&, int, int); //should probably pass ref to struct instead of individual values

    struct RenderBuffer
    {
        //Pixels are 32-bit | Mem Order: BB GG RR XX | Little Endian: XX RR GG BB
        BITMAPINFO info; //windows.h is included just for this, change?
        void *memory;    //should use smart pointer here just because
        int width;
        int height;
        int bytesPerRow;
    };

    class Renderer
    {
        private:
            RenderBuffer _buffer;
            Win32DisplayBufferFunc _copyBufferToScreen;

            void AllocateRenderBuffer(int t_width, int t_height);

            void RenderTileMap();
            void RenderSprites(std::vector<NCE::Common::Rect> &t_spriteRects);

            int GetTileMapValueAtCoordinates(int t_x, int t_y, int t_tileWidth);

        public:
            Renderer(Win32DisplayBufferFunc t_displayBuffer, int t_initialWidth, int t_initialHeight);

            void StartRenderCycle(std::vector<NCE::Common::Rect> &t_spriteRects);
            void ForceRender();
            

            
    };
}


#endif