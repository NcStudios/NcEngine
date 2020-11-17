#pragma once
#include "win32/NcWin32.h"
#include "math/Vector2.h"

#include <functional>

namespace nc
{
    namespace engine { class ApiBinder; }

    class Window
    {
        public:
            static Vector2 GetDimensions();

        private:
            friend class nc::engine::ApiBinder;
            static std::function<Vector2()> GetDimensions_;
    };

} //end namespace nc