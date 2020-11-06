#pragma once

namespace nc::ui
{
    class IUI
    {
        public:
            virtual void Draw() = 0;
            virtual bool IsHovered() = 0;
    };
}