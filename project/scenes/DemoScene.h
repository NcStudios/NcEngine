#pragma once

#include "scene/Scene.h"
#include "project/source/log/GameLog.h"
#include "project/source/ui/Hud.h"

#include <memory>

namespace project
{
    class DemoScene : public nc::scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;

        private:
            std::unique_ptr<project::ui::Hud> m_hud;
            std::unique_ptr<project::log::GameLog> m_log;
    };
}