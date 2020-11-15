#include "UI.h"
#include "DebugUtils.h"

namespace nc::ui
{
    std::function<void(IUI*)> UI::Set_ = nullptr;
    std::function<bool()> UI::IsHovered_ = nullptr;

    void UI::Set(IUI* ui)
    {
        V_LOG("Registering project UI");
        UI::Set_(ui);
    }

    bool UI::IsHovered()
    {
        return UI::IsHovered_();
    }
}