#pragma once
#include "scene/Scene.h"

#include <functional>
#include <memory>

namespace nc::engine { class ApiBinder; }

namespace nc::scene
{
    class SceneManager
    {
        public:
            static void ChangeScene(std::unique_ptr<scene::Scene>&& scene);

        private:
            friend class ::nc::engine::ApiBinder;
            static std::function<void(std::unique_ptr<scene::Scene>&&)> ChangeScene_;
    };
}