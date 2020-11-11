#include "NcUI.h"
#include "ui/UISystem.h"
#include "NcDebug.h"

namespace nc::ui
{
    void NcRegisterUI(IUI* ui)
    {
        V_LOG("Registering project UI");
        UISystem::BindProjectUI(ui);
    }

    bool NcIsUIHovered()
    {
        return UISystem::IsProjectUIHovered();
    }
}