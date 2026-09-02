#include "GraphicsTypes.h"
#include "ncengine/graphics/BoneSnapper.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/WireframeRenderer.h"

namespace nc
{
void RegisterGraphicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<StaticMesh>(
        registry,
        maxEntities,
        StaticMeshId,
        "StaticMesh",
        ui::editor::StaticMeshUIWidget,
        CreateStaticMesh,
        SerializeStaticMesh,
        DeserializeStaticMesh
    );

    Register<SkinnedMesh>(
        registry,
        maxEntities,
        SkinnedMeshId,
        "SkinnedMesh",
        ui::editor::SkinnedMeshUIWidget,
        CreateSkinnedMesh,
        SerializeSkinnedMesh,
        DeserializeSkinnedMesh
    );

    Register<ParticleEmitter>(
        registry,
        maxEntities,
        ParticleEmitterId,
        "ParticleEmitter",
        ui::editor::ParticleEmitterUIWidget,
        CreateParticleEmitter,
        SerializeParticleEmitter,
        DeserializeParticleEmitter
    );

    Register<PointLight>(
        registry,
        maxEntities,
        PointLightId,
        "PointLight",
        ui::editor::PointLightUIWidget,
        CreatePointLight,
        SerializePointLight,
        DeserializePointLight
    );

    Register<SpotLight>(
        registry,
        maxEntities,
        SpotLightId,
        "SpotLight",
        ui::editor::SpotLightUIWidget,
        CreateSpotLight,
        SerializeSpotLight,
        DeserializeSpotLight
    );

    Register<DirectionalLight>(
        registry,
        maxEntities,
        DirectionalLightId,
        "DirectionalLight",
        ui::editor::DirectionalLightUIWidget,
        CreateDirectionalLight,
        SerializeDirectionalLight,
        DeserializeDirectionalLight
    );

    Register<WireframeRenderer>(
        registry,
        maxEntities,
        WireframeRendererId,
        "WireframeRenderer"
    );

    Register<BoneSnapper>(
        registry,
        maxEntities,
        BoneSnapperId,
        "BoneSnapper",
        ui::editor::BoneSnapperUIWidget,
        CreateBoneSnapper
    );
};
} // namespace nc
