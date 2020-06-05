#pragma once
#include <vector>
#include <memory>
#include "NCWinDef.h"

#include "Renderer.h"
#include "ComponentSystem.h"

namespace nc
{ 
    class Camera;
    namespace graphics { class Graphics; }
}

namespace nc::engine
{
    class RenderingSystem : public ComponentSystem<Renderer>
    {
        public:
            RenderingSystem(int initialWidth, int initialHeight, HWND hwnd);
            ~RenderingSystem();
            RenderingSystem(const RenderingSystem&) = delete;
            RenderingSystem(RenderingSystem&&) = delete;

            void FrameBegin();
            void Frame();
            void FrameEnd();

            nc::graphics::Graphics* GetGraphics();

        private:
            std::unique_ptr<graphics::Graphics> m_graphics;
    };
} //end namespace nc::engine