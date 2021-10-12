#include "DefaultComponents.h"
#include "Assets.h"

namespace nc::editor
{
    void AddDefaultMeshRenderer(registry_type* registry, Entity entity)
    {
        registry->Add<MeshRenderer>(entity, CubeMeshPath, DefaultMaterial, graphics::TechniqueType::PhongAndUi);
    }

    void AddDefaultHullCollider(registry_type* registry, Entity entity, bool isTrigger)
    {
        registry->Add<Collider>(entity, HullProperties{.assetPath = CubeHullColliderPath}, isTrigger);
    }

    void AddDefaultConcaveCollider(registry_type* registry, Entity entity)
    {
        registry->Add<ConcaveCollider>(entity, PlaneConcaveColliderPath);
    }
}