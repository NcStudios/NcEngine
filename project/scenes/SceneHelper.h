#pragma once

#include "Ecs.h"
#include "MainCamera.h"
#include "UI.h"
#include "source/log/GameLog.h"
#include "source/ui/SampleUI.h"
#include "components/FrameTimer.h"

#include <functional>
#include <memory>

namespace project
{
    /** Performs common setup for sample scenes. */
    class SceneHelper
    {
        public:
            void Setup(bool enableLog = true, bool createLight = true, std::function<void()> widgetCallback = nullptr)
            {
                if(enableLog)
                {
                    m_log = std::make_unique<log::GameLog>();
                    auto timerHandle = nc::CreateEntity("FrameTimer");
                    AddComponent<FrameTimer>(timerHandle);
                }

                // UI
                m_ui = std::make_unique<project::ui::SampleUI>(m_log.get(), widgetCallback);
                nc::ui::Set(m_ui.get());

                // Camera
                auto camHandle = CreateEntity(nc::Vector3::Back() * 10.0f, nc::Quaternion::Identity(), nc::Vector3::One(), "Main Camera");
                auto camComponentPtr = AddComponent<nc::Camera>(camHandle);
                nc::camera::SetMainCamera(camComponentPtr);

                // Light
                if(createLight)
                {
                    auto lvHandle = CreateEntity(nc::Vector3::Up() * 12.0f, nc::Quaternion::Identity(), nc::Vector3::One(), "Point Light");
                    AddComponent<nc::PointLight>(lvHandle);
                }
            }

            void TearDown()
            {
                nc::ui::Set(nullptr);
                m_ui = nullptr;
                m_log = nullptr;
            }

        private:
            std::unique_ptr<ui::SampleUI> m_ui;
            std::unique_ptr<log::GameLog> m_log;
    };
}