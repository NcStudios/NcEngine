#pragma once
#include "win32/NcWinDef.h"

namespace nc
{
    namespace config::detail { struct ConfigPaths; }

    namespace engine
    {
        /**
         * Initialize the window and engine state.
         * @param hInstance Handle to the process instance passed to WinMain.
         * @param configPaths Paths to configuration files.
         * @throw std::runtime_error if attempting to 
         * @note A call to this should have a matching call to NcShutDownEngine.
         */
        void NcInitializeEngine(HINSTANCE hInstance, const config::detail::ConfigPaths& configPaths);

        /**
         * Start the engine's game loop.
         * @throw std::runtime_error if the engine has not been initialized.
         */
        void NcStartEngine();

        /**
         * Stop the engine and clean up state.
         * @throw std::runtime_error if engine is not initialized.
         */
        void NcShutdownEngine();
    }
}