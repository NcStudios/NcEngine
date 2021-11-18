#pragma once

#include "ecs/Registry.h"

#include "ecs/component/AudioSource.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/NetworkDispatcher.h"
#include "ecs/component/ParticleEmitter.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/PointLight.h"
#include "ecs/component/Transform.h"

namespace nc::editor
{
    const auto CubeMeshPath             = std::string{"cube.nca"};
    const auto CapsuleMeshPath          = std::string{"capsule.nca"};
    const auto SphereMeshPath           = std::string{"sphere.nca"};
    const auto PlaneMeshPath            = std::string{"plane.nca"};
    const auto CubeHullColliderPath     = std::string{"cube.nca"};
    const auto PlaneConcaveColliderPath = std::string{"plane.nca"};
    const auto DefaultBaseColorPath     = std::string{"DefaultBaseColor.png"};
    const auto DefaultNormalPath        = std::string{"DefaultNormal.png"};
    const auto DefaultRoughnessPath     = std::string{"DefaultMetallic.png"};
    const auto DefaultMaterial          = nc::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath, DefaultRoughnessPath};

    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(Registry* registry, Entity entity);
    void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger);
    void AddDefaultConcaveCollider(Registry* registry, Entity entity);
}