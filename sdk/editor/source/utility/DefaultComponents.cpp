#include "DefaultComponents.h"
#include "asset/Assets.h"

namespace nc::editor
{
    void AddDefaultMeshRenderer(Registry* registry, Entity entity)
    {
        registry->Add<MeshRenderer>(entity, CubeMeshPath, DefaultMaterial, TechniqueType::PhongAndUi);
    }

    void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger)
    {
        registry->Add<physics::Collider>(entity, physics::HullProperties{.assetPath = CubeHullColliderPath}, isTrigger);
    }

    void AddDefaultConcaveCollider(Registry* registry, Entity entity)
    {
        registry->Add<ConcaveCollider>(entity, PlaneConcaveColliderPath);
    }
}