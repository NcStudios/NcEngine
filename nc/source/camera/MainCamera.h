#pragma once

namespace nc { class Camera; }

namespace nc::camera
{
    class MainCamera final
    {
        public:
            void Set(Camera* camera) noexcept;
            auto Get() noexcept -> Camera*;
        
        private:
            Camera* m_camera = nullptr;
    };
}