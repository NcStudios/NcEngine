#pragma once

#include "ecs/Registry.h"

#include "audio/AudioSource.h"
#include "ecs/Logic.h"
#include "ecs/Tag.h"
#include "ecs/Transform.h"
#include "ecs/detail/FreeComponentGroup.h"
#include "graphics/Camera.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/PointLight.h"
#include "network/NetworkDispatcher.h"
#include "physics/Collider.h"
#include "physics/ConcaveCollider.h"
#include "physics/PhysicsBody.h"

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
    const auto DefaultMaterial          = nc::graphics::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath, DefaultRoughnessPath};
    const auto DefaultSkyboxPath        = std::string{"DefaultSkybox\\DefaultSkybox.nca"};

    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(Registry* registry, Entity entity);
    void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger);
    void AddDefaultConcaveCollider(Registry* registry, Entity entity);
}