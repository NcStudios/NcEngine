#pragma once

#include "NcEngine.h"
#include "ui/IUI.h"
#include "Window.h"
#include "shared/GameLog.h"

#include <functional>

namespace nc::sample
{
    class SampleUI : public ui::IUI, public window::IOnResizeReceiver
    {
        public:
            SampleUI(NcEngine* engine);
            ~SampleUI() noexcept;
            void Draw() override;
            bool IsHovered() override;
            void OnResize(Vector2 dimensions) override;
            void SetWidgetCallback(std::function<void()> func);

        private:
            NcEngine* m_engine;
            GameLog m_gameLog;
            std::function<void()> m_widgetCallback;
            Vector2 m_windowDimensions;

            void CheckInput();
            void DrawDefaultWidget();
            void DrawLog();
            void DrawSceneList();
    };

    auto InitializeSampleUI(NcEngine* engine) -> std::unique_ptr<SampleUI>;
} //end namespace project::ui