#pragma once
#include "component/Camera.h"

#include <functional>

namespace nc
{
    class Transform;
    namespace engine { class ApiBinder; }
}

namespace nc::camera
{
    class MainCamera
    {
        public:
            static void Set(Camera* camera);
            static Transform* GetTransform();

        private:
            friend class ::nc::engine::ApiBinder;
            static std::function<void(Camera*)> Set_;
            static std::function<Transform*()> GetTransform_;
    };
}