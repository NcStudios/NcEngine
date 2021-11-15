#include "DefaultComponents.h"
#include "Assets.h"

namespace nc::editor
{
    void AddDefaultMeshRenderer(Registry* registry, Entity entity)
    {
        registry->Add<MeshRenderer>(entity, CubeMeshPath, DefaultMaterial, TechniqueType::PhongAndUi);
    }

    void AddDefaultHullCollider(Registry* registry, Entity entity, bool isTrigger)
    {
        registry->Add<Collider>(entity, HullProperties{.assetPath = CubeHullColliderPath}, isTrigger);
    }

    void AddDefaultConcaveCollider(Registry* registry, Entity entity)
    {
        registry->Add<ConcaveCollider>(entity, PlaneConcaveColliderPath);
    }
}