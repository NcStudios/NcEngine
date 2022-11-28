#include "DefaultComponents.h"

#include "ncengine/asset/Assets.h"

namespace nc::editor
{
void AddDefaultMeshRenderer(Registry* registry, Entity entity)
{
    registry->Add<graphics::MeshRenderer>(entity, CubeMeshPath, DefaultMaterial, graphics::TechniqueType::PhongAndUi);
}

void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger)
{
    registry->Add<physics::Collider>(entity, physics::HullProperties{.assetPath = CubeHullColliderPath}, isTrigger);
}

void AddDefaultConcaveCollider(Registry* registry, Entity entity)
{
    registry->Add<physics::ConcaveCollider>(entity, PlaneConcaveColliderPath);
}
}
