#pragma once

#include "ui/IUI.h"
#include "Window.h"
#include "source/log/GameLog.h"

#include <functional>

namespace project::ui
{
    class SampleUI : public nc::ui::IUI, public nc::window::IOnResizeReceiver
    {
        public:
            SampleUI(log::GameLog* gameLog = nullptr, std::function<void()> widgetCallback = nullptr);
            ~SampleUI();
            void Draw() override;
            bool IsHovered() override;
            void OnResize(nc::Vector2 dimensions) override;

        private:
            log::GameLog* m_gameLog;
            std::function<void()> m_widgetCallback;
            nc::Vector2 m_windowDimensions;
            bool m_isHovered;

            void CheckInput();
            void DrawDefaultWidget();
            void DrawLog();
            void DrawSceneList();
    };
} //end namespace project::ui