#pragma once
#include <vector>
#include <memory>
#include "win32/NCWinDef.h"

#include "component/Renderer.h"
#include "ComponentSystem.h"

namespace nc
{ 
    class Camera;
    namespace graphics { class Graphics; }
}

namespace nc::ecs
{
    class RenderingSystem : public ComponentSystem<Renderer>
    {
        public:
            RenderingSystem(HWND hwnd, float width, float height, float nearZ, float farZ, bool fullscreen);
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