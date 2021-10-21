#pragma once

#include "Scene.h"
#include "ui/IUI.h"
#include "Window.h"
#include "shared/GameLog.h"

#include <functional>

namespace nc::sample
{
    class SampleUI : public ui::IUI, public window::IOnResizeReceiver
    {
        public:
            SampleUI(SceneSystem* sceneSystem, GameLog* gameLog = nullptr, std::function<void()> widgetCallback = nullptr);
            ~SampleUI() noexcept;
            void Draw() override;
            bool IsHovered() override;
            void OnResize(Vector2 dimensions) override;

        private:
            SceneSystem* m_sceneSystem;
            GameLog* m_gameLog;
            std::function<void()> m_widgetCallback;
            Vector2 m_windowDimensions;

            void CheckInput();
            void DrawDefaultWidget();
            void DrawLog();
            void DrawSceneList();
    };
} //end namespace project::ui