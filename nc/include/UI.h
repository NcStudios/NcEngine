#pragma once

#include <functional>

namespace nc::engine { class ApiBinder; }
namespace nc::ui { class IUIBase; }

namespace nc::ui
{
    class UI
    {
        public:
            static void Set(IUIBase* ui);
            static bool IsHovered();

        private:
            friend class nc::engine::ApiBinder;
            static std::function<void(IUIBase*)> Set_;
            static std::function<bool()> IsHovered_;
    };
}