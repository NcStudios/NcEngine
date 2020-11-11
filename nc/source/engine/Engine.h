#pragma once

#include "win32/HInstanceForwardDecl.h"

#include <memory>

namespace nc::engine
{
    class EngineImpl;

    class Engine
    {
        public:
            Engine(HINSTANCE hInstance);
            ~Engine();
            void Start();
            void Shutdown(bool forceImmediate = false);

        private:
            std::unique_ptr<EngineImpl> m_impl;
    };
}