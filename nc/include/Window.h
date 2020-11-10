#pragma once
#include "win32/NcWin32.h"
#include "math/Vector2.h"

namespace nc
{
    namespace window { class WindowImpl; }

    class Window
    {
        public:
            static void RegisterImpl(window::WindowImpl* impl);

            static Vector2 GetDimensions();

        private:
            static window::WindowImpl* m_impl;
    };

} //end namespace nc