#pragma once

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

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