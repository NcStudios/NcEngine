#pragma once
#include "win32/NcWinDef.h"

namespace nc
{
    namespace engine
    {
        /**
         * Initialize the window and engine state.
         * @param hInstance Handle to the process instance passed to WinMain.
         * @param configPaths Paths to configuration files.
         * @throw std::runtime_error if attempting to 
         * @note A call to this should have a matching call to NcShutDownEngine.
         */
        void NcInitializeEngine(HINSTANCE hInstance);

        /**
         * Start the engine's game loop.
         * @throw std::runtime_error if the engine has not been initialized.
         */
        void NcStartEngine();

        /**
         * Sets the engine to an uninitialized state after the current loop finishes 
         * or immediately, depending on the value of forceImmediate.
         * @param forceImmediate Should be false for typical shutdown. In the case
         * of an irrecoverable exception, set to true to immediately wipe state as
         * the engine will not be able to.
         */
        void NcShutdownEngine(bool forceImmediate = false);
    }
}