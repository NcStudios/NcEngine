#pragma once

#include "scene/Scene.h"
#include "project/source/ui/MainMenuUI.h"

#include <memory>

namespace project
{
    class MenuScene : public nc::scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
        
        private:
            std::unique_ptr<project::ui::MainMenuUI> m_ui;
    };
}