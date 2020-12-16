#pragma once

#include "nc/source/scene/Scene.h"
#include "source/log/GameLog.h"
#include "source/ui/Hud.h"

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