#pragma once

#include "win32/HInstanceForwardDecl.h"
#include "config/Config.h"
#include "scene/Scene.h"

#include <functional>
#include <memory>

namespace nc::engine
{
    class ApiBinder;
    class EngineImpl;

    class Engine
    {
        public:
            Engine(HINSTANCE hInstance);
            ~Engine();
            void Start(std::unique_ptr<scene::Scene> initialScene);
            void Shutdown(bool forceImmediate = false);

            static const config::Config& GetConfig();
            static void SetUserName(std::string name);

        private:
            std::unique_ptr<EngineImpl> m_impl;
            
            friend class ApiBinder;
            static std::function<const config::Config&()> GetConfig_;
            static std::function<void(std::string)> SetUserName_;
    };
}