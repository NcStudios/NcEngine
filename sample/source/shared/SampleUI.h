#pragma once

#include "shared/GameLog.h"

#include "ncengine/NcEngine.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/ui/IUI.h"
#include "ncengine/window/Window.h"

#include <functional>

namespace nc::sample
{
class SampleUI : public ui::IUI, public window::IOnResizeReceiver
{
    public:
        SampleUI(NcScene* ncScene);
        ~SampleUI() noexcept;
        void Draw() override;
        bool IsHovered() override;
        void OnResize(Vector2 dimensions) override;
        void SetWidgetCallback(std::function<void()> func);

    private:
        NcScene* m_ncScene;
        GameLog m_gameLog;
        std::function<void()> m_widgetCallback;
        Vector2 m_windowDimensions;
        Vector2 m_screenExtent;
        FontView m_font;

        void CheckInput();
        void DrawDefaultWidget();
        void DrawLog();
        void DrawSceneList();
};

auto InitializeSampleUI(NcEngine* engine) -> std::unique_ptr<SampleUI>;
} // namespace nc::sample
