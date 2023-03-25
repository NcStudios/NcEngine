#pragma once

#include <optional>

#include "graphics/DebugWidget.h"
#include "ecs/View.h"
#include "physics/Collider.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct WidgetState
{
    std::optional<DebugWidget> selectedCollider;
};

class WidgetSystem
{
    public:
        auto Execute(View<physics::Collider> colliders) -> WidgetState;
};
} // namespace nc::graphics
