#pragma once

#include "scene/Scene.h"
#include "project/source/log/GameLog.h"
#include "project/source/ui/UI.h"

#include <memory>

class GameScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;

    private:
        std::unique_ptr<project::ui::UI> m_ui;
        std::unique_ptr<project::log::GameLog> m_log;
};