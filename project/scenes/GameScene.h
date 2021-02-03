#pragma once

#include "Scene.h"
#include "source/log/GameLog.h"
#include "source/ui/Hud.h"

#include <memory>
#include <string>

namespace project
{
    class GameScene : public nc::scene::Scene
    {
        public:
            GameScene(std::string serverIP);
            void Load() override;
            void Unload() override;

        private:
            std::unique_ptr<project::ui::Hud> m_hud;
            std::unique_ptr<project::log::GameLog> m_log;
            std::string m_serverIP;
    };
}