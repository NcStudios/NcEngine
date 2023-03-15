#include "WidgetSystem.h"

namespace nc::graphics
{
auto WidgetSystem::Execute([[maybe_unused]] View<physics::Collider> colliders) -> WidgetState
{
    auto state = WidgetState{std::nullopt};

#ifdef NC_EDITOR_ENABLED
    for (auto& collider : colliders)
    {
        if (collider.GetEditorSelection())
        {
            state.selectedCollider = collider.GetDebugWidget();
        }

        collider.SetEditorSelection(false);
    }
#endif

    return state;
}
} // namespace nc::graphics
