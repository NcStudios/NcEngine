#pragma once

#include "MainCamera.h"

namespace nc::camera
{
    class MainCameraImpl final : public MainCamera
    {
        public:
            void Set(Camera* camera) override;
            auto Get() -> Camera* override;
        
        private:
            Camera* m_camera = nullptr;
    };
}