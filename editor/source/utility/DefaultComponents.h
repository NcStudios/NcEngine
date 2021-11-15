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
    const auto CubeMeshPath             = std::string{"nc/resources/mesh/cube.nca"};
    const auto CapsuleMeshPath          = std::string{"nc/resources/mesh/capsule.nca"};
    const auto SphereMeshPath           = std::string{"nc/resources/mesh/sphere.nca"};
    const auto PlaneMeshPath            = std::string{"nc/resources/mesh/plane.nca"};
    const auto CubeHullColliderPath     = std::string{"nc/resources/hull_collider/cube.nca"};
    const auto PlaneConcaveColliderPath = std::string{"nc/resources/concave_collider/plane.nca"};
    const auto DefaultBaseColorPath     = std::string{"nc/resources/texture/DefaultBaseColor.png"};
    const auto DefaultNormalPath        = std::string{"nc/resources/texture/DefaultNormal.png"};
    const auto DefaultRoughnessPath     = std::string{"nc/resources/texture/DefaultMetallic.png"};
    const auto DefaultMaterial          = nc::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath, DefaultRoughnessPath};

    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(Registry* registry, Entity entity);
    void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger);
    void AddDefaultConcaveCollider(Registry* registry, Entity entity);
}