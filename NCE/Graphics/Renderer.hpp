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
            RenderBuffer m_buffer;
            Win32DisplayBufferFunc CopyBufferToScreen;

            void AllocateRenderBuffer(int width_, int height_);

            void RenderTileMap();
            void RenderSprites(std::vector<NCE::Common::Rect> &spriteRects_);

            int GetTileMapValueAtCoordinates(int x_, int y_, int tileWidth_);

        public:
            Renderer(Win32DisplayBufferFunc displayBuffer_, int initialWidth_, int initialHeight_);

            void StartRenderCycle(std::vector<NCE::Common::Rect> &spriteRects_);
            void ForceRender();
            

            
    };
}


#endif