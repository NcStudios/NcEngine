#pragma once

#include <functional>

namespace nc::engine { class ApiBinder; }
namespace nc::ui { class IUI; }

namespace nc::ui
{
    class UI
    {
        public:
            static void Set(IUI* ui);

            [[nodiscard]] static bool IsHovered();

        private:
            friend class nc::engine::ApiBinder;
            static std::function<void(IUI*)> Set_;
            static std::function<bool()> IsHovered_;
    };
}