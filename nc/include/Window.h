#pragma once
#include "win32/NcWin32.h"
#include "math/Vector2.h"

#include <functional>

namespace nc
{
    namespace engine { class ApiBinder; }
    namespace window { class IOnResizeReceiver; }

    class Window
    {
        public:
            static Vector2 GetDimensions();

            static void RegisterOnResizeReceiver(window::IOnResizeReceiver* receiver);
            static void UnregisterOnResizeReceiver(window::IOnResizeReceiver* receiver);

        private:
            friend class nc::engine::ApiBinder;
            static std::function<Vector2()> GetDimensions_;
            static std::function<void(window::IOnResizeReceiver*)> RegisterOnResizeReceiver_;
            static std::function<void(window::IOnResizeReceiver*)> UnregisterOnResizeReceiver_;
    };

} //end namespace nc