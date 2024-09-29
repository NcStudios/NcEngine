#pragma once

#include "ncengine/ecs/EcsFwd.h"

namespace nc::physics
{
class ContactListener;

void DispatchPhysicsEvents(ContactListener& events, ecs::Ecs world);
} // namespace nc::physics
