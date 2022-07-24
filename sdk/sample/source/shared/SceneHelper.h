#pragma once

#include "ModuleRegistry.h"
#include "ecs/Registry.h"
#include "ecs/component/PointLight.h"
#include "graphics/GraphicsModule.h"

#include "GameLog.h"
#include "SampleUI.h"

#include <functional>
#include <memory>

namespace nc::sample
{
    /** Performs common setup for sample scenes. */
    class SceneHelper
    {
        public:
            void Setup(Registry* registry, ModuleRegistry* modules, bool enableLog = true, bool createLight = true, std::function<void()> widgetCallback = nullptr);
            void TearDown();

        private:
            std::unique_ptr<SampleUI> m_ui;
            std::unique_ptr<GameLog> m_log;
            ModuleRegistry* m_modules;
    };

    inline void SceneHelper::Setup(Registry* registry, ModuleRegistry* modules, bool enableLog, bool createLight, std::function<void()> widgetCallback)
    {
        m_modules = modules;

        if(enableLog)
        {
            m_log = std::make_unique<GameLog>();
        }

        m_ui = std::make_unique<SampleUI>(modules->Get<SceneModule>(), m_log.get(), widgetCallback);
        modules->Get<GraphicsModule>()->SetUi(m_ui.get());

        if(createLight)
        {
            auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
            registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(1.0f, 0.7f, 1.0f),
                                                               .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                                               .diffuseIntensity = 5.0f});
        }
    }

    inline void SceneHelper::TearDown()
    {
        m_modules->Get<GraphicsModule>()->SetUi(nullptr);
        m_ui = nullptr;
        m_log = nullptr;
    }
}