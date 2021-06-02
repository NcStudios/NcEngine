#pragma once

#include "Entity.h"
#include "component/AutoComponentGroup.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/ParticleEmitter.h"
#include "component/PointLight.h"
#include "component/Registry.h"
#include "component/Renderer.h"
#include "component/Tag.h"
#include "component/Transform.h"

namespace nc
{
    using registry_type = ecs::Registry<AutoComponentGroup, Collider, NetworkDispatcher, ParticleEmitter, PointLight, Renderer, Tag, Transform>;

    auto ActiveRegistry() -> registry_type*;

    namespace internal
    {
        void SetActiveRegistry(registry_type* registry);
    }
} // end namespace nc