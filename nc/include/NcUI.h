#pragma once

#include "ui/IUI.h"

namespace nc::ui
{
    void NcRegisterUI(IUI* ui);
    bool NcIsUIHovered();
}