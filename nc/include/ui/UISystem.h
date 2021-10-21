#pragma once

#include "utility/SystemBase.h"
#include "ui/IUI.h"

namespace nc
{
    class UISystem : public SystemBase
    {
        public:
            /** Set an IUI to receive Draw() callbacks each frame. Pass
            *  nullptr to unregister the ui. */
            virtual void Set(ui::IUI* ui) noexcept = 0;

            /** Same as calling IsHovered() directly on the registered IUI. */
            virtual bool IsHovered() const noexcept = 0;
    };
}