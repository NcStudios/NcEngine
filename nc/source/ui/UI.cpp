#include "UI.h"
#include "DebugUtils.h"

namespace nc::ui
{
    std::function<void(IUIBase*)> UI::Set_ = nullptr;
    std::function<bool()> UI::IsHovered_ = nullptr;

    void UI::Set(IUIBase* ui)
    {
        V_LOG("Registering project UI");
        IF_THROW(!UI::Set_, "UI::Set_ is not bound");
        UI::Set_(ui);
    }

    bool UI::IsHovered()
    {
        IF_THROW(!UI::IsHovered_, "UI::IsHovered_ is not bound");
        return UI::IsHovered_();
    }
}