#pragma once

#include "Scene.h"
#include "source/ui/MainMenuUI.h"

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