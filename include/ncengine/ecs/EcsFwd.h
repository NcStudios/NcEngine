/**
 * @file EcsFwd.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/EcsFilter.h"

namespace nc::ecs
{
template<FilterBase Base, class... Includes>
class EcsInterface;

using Ecs = EcsInterface<FilterBase::All>;

template<class... Includes>
using BasicEcs = EcsInterface<FilterBase::Basic, Includes...>;

template<class... Includes>
using ExplicitEcs = EcsInterface<FilterBase::None, Includes...>;
} // namespace nc::ecs
