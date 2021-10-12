#pragma once

#include "Ecs.h"

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
    const auto DefaultMaterial          = nc::graphics::Material{DefaultBaseColorPath, DefaultNormalPath, DefaultRoughnessPath};

    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(registry_type* registry, Entity entity);
    void AddDefaultHullCollider(registry_type* registry, Entity entity, bool isTrigger);
    void AddDefaultConcaveCollider(registry_type* registry, Entity entity);
}