#pragma once

#include "Ecs.h"
#include "UI.h"
#include "GameLog.h"
#include "SampleUI.h"
#include "FrameTimer.h"

#include <functional>
#include <memory>

namespace nc::sample
{
    /** Performs common setup for sample scenes. */
    class SceneHelper
    {
        public:
            void Setup(registry_type* registry, bool enableLog = true, bool createLight = true, std::function<void()> widgetCallback = nullptr);
            void TearDown();

        private:
            std::unique_ptr<SampleUI> m_ui;
            std::unique_ptr<GameLog> m_log;
    };

    inline void SceneHelper::Setup(registry_type* registry, bool enableLog, bool createLight, std::function<void()> widgetCallback)
    {
        if(enableLog)
        {
            m_log = std::make_unique<GameLog>();
            auto timerHandle = registry->Add<Entity>({.tag = "FrameTimer"});
            registry->Add<FrameTimer>(timerHandle);
        }

        m_ui = std::make_unique<SampleUI>(m_log.get(), widgetCallback);
        ui::Set(m_ui.get());

        if(createLight)
        {
            auto lvHandle = registry->Add<Entity>({.position = Vector3::Up() * 12.0f, .tag = "Point Light"});
            registry->Add<PointLight>(lvHandle, PointLight::Properties{});
        }
    }

    inline void SceneHelper::TearDown()
    {
        ui::Set(nullptr);
        m_ui = nullptr;
        m_log = nullptr;
    }
}