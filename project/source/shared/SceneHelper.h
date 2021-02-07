#pragma once

#include "Ecs.h"
#include "MainCamera.h"
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
            void Setup(bool enableLog = true, bool createLight = true, std::function<void()> widgetCallback = nullptr);
            void TearDown();

        private:
            std::unique_ptr<SampleUI> m_ui;
            std::unique_ptr<GameLog> m_log;
    };

    inline void SceneHelper::Setup(bool enableLog, bool createLight, std::function<void()> widgetCallback)
    {
        if(enableLog)
        {
            m_log = std::make_unique<GameLog>();
            auto timerHandle = CreateEntity("FrameTimer");
            AddComponent<FrameTimer>(timerHandle);
        }

        m_ui = std::make_unique<SampleUI>(m_log.get(), widgetCallback);
        ui::Set(m_ui.get());

        auto camHandle = CreateEntity(Vector3::Back() * 10.0f, Quaternion::Identity(), Vector3::One(), "Main Camera");
        auto camComponentPtr = AddComponent<Camera>(camHandle);
        nc::camera::SetMainCamera(camComponentPtr);

        if(createLight)
        {
            auto lvHandle = CreateEntity(Vector3::Up() * 12.0f, Quaternion::Identity(), Vector3::One(), "Point Light");
            AddComponent<PointLight>(lvHandle);
        }
    }

    inline void SceneHelper::TearDown()
    {
        ui::Set(nullptr);
        m_ui = nullptr;
        m_log = nullptr;
    }
}