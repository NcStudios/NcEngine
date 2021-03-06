#pragma once

#include "ui/IUI.h"
#include "Window.h"
#include "shared/GameLog.h"

#include <functional>

namespace nc::sample
{
    class SampleUI : public ui::IUI, public window::IOnResizeReceiver
    {
        public:
            SampleUI(GameLog* gameLog = nullptr, std::function<void()> widgetCallback = nullptr);
            ~SampleUI() noexcept;
            void Draw() override;
            bool IsHovered() override;
            void OnResize(Vector2 dimensions) override;

        private:
            GameLog* m_gameLog;
            std::function<void()> m_widgetCallback;
            Vector2 m_windowDimensions;
            bool m_isHovered;

            void CheckInput();
            void DrawDefaultWidget();
            void DrawLog();
            void DrawSceneList();
    };
} //end namespace project::ui