#pragma once

#include "Entity.h"
#include "component/AudioSource.h"
#include "component/AutoComponentGroup.h"
#include "component/Collider.h"
#include "component/ConcaveCollider.h"
#include "component/NetworkDispatcher.h"
#include "component/ParticleEmitter.h"
#include "component/PhysicsBody.h"
#include "component/Registry.h"
#include "component/Tag.h"
#include "component/Transform.h"
#include "component/MeshRenderer.h"
#include "component/PointLight.h"

namespace nc
{
    using registry_type = Registry;

    auto ActiveRegistry() -> registry_type*;

    namespace internal
    {
        void SetActiveRegistry(registry_type* registry);
    }
} // end namespace nc