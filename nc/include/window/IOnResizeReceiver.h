#pragma once

#include "math/Vector.h"

namespace nc::window
{
    class IOnResizeReceiver
    {
        public:
            virtual void OnResize(Vector2 dimensions) = 0;
        
        protected:
            ~IOnResizeReceiver() = default;
    };
}