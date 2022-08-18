#pragma once

namespace nc::graphics
{
class Camera;
}

namespace nc::camera
{
    class MainCamera final
    {
        public:
            void Set(graphics::Camera* camera) noexcept;
            auto Get() noexcept -> graphics::Camera*;
        
        private:
            graphics::Camera* m_camera = nullptr;
    };
}