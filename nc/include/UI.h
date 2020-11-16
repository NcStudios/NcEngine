#pragma once

#include "ui/IUI.h"
#include "math/Vector2.h"

#include <functional>

namespace nc::engine { class ApiBinder; }

namespace nc::ui
{
    class UI
    {
        public:
            static void Set(IUI* ui);
            static bool IsHovered();

        private:
            friend class nc::engine::ApiBinder;
            static std::function<void(IUI*)> Set_;
            static std::function<bool()> IsHovered_;
    };

    struct Utils
    {
        static inline Vector2 GetTopLeftToCenterElement(Vector2 screenDimensions, Vector2 elementDimensions)
        {
            return (screenDimensions - elementDimensions) / 2.0f;
        }
    };
}