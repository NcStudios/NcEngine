#pragma once

#include "platform/win32/HInstanceForwardDecl.h"
#include "config/Config.h"
#include "Scene.h"

namespace nc
{
    class Engine;

    /** Core engine object. */
    class NcEngine
    {
        public:
            /** Constructing an NcEngine object will initialize all engine state. Editor
             *  mode should not be used by games. */
            NcEngine(HINSTANCE hInstance, bool useEditorMode = false);

            /** Calls Shutdown. */
            ~NcEngine() noexcept;

            /** Load initial scene and start game loop. */
            void Start(std::unique_ptr<scene::Scene> initialScene);
            
            /** Stop the game loop after the current frame has finished. Do not call
             *  if an exception is thrown from Start. */
            void Quit() noexcept;

            /** Save configuration and destroy internal state. */
            void Shutdown() noexcept;

            /** For internal use. */
            auto GetImpl() noexcept -> Engine*;

        private:
            std::unique_ptr<Engine> m_impl;
    };
}