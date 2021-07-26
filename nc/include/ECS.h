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
#include "component/vulkan/DebugWidget.h"
#include "component/vulkan/MeshRenderer.h"
#include "component/vulkan/PointLight.h"

namespace nc
{
    using registry_type_list = ecs::RegistryTypeList<AutoComponentGroup, Collider, NetworkDispatcher, ParticleEmitter, PointLight, Renderer, Tag, Transform,  vulkan::DebugWidget, vulkan::MeshRenderer, vulkan::PointLight>;
    using registry_type = ecs::Registry<registry_type_list>;

    auto ActiveRegistry() -> registry_type*;

    namespace internal
    {
        void SetActiveRegistry(registry_type* registry);
    }
} // end namespace nc