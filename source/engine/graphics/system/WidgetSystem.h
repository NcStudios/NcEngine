#pragma once

#include <optional>

#include "graphics/DebugWidget.h"
#include "ecs/View.h"
#include "physics/Collider.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct WidgetSystemState
{
    std::optional<DebugWidget> selectedCollider;
};

class WidgetSystem
{
    public:
        auto Execute(View<physics::Collider> colliders) -> WidgetSystemState;
};
} // namespace nc::graphics
