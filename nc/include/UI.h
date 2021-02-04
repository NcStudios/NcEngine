#pragma once

#include "ui/IUI.h"

namespace nc::ui
{
    /** Set an IUI to receive Draw() callbacks each frame. Pass
     *  nullptr to unregister the ui. */
    void Set(IUI* ui);

    /** Same as calling IsHovered() directly on the registered IUI. */
    [[nodiscard]] bool IsHovered();
}