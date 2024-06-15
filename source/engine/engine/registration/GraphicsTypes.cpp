#include "GraphicsTypes.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/WireframeRenderer.h"

namespace nc
{
void RegisterGraphicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<graphics::MeshRenderer>(registry, maxEntities, MeshRendererId, "MeshRenderer", ui::editor::MeshRendererUIWidget, CreateMeshRenderer, SerializeMeshRenderer, DeserializeMeshRenderer);
    Register<graphics::ToonRenderer>(registry, maxEntities, ToonRendererId, "ToonRenderer", ui::editor::ToonRendererUIWidget, CreateToonRenderer, SerializeToonRenderer, DeserializeToonRenderer);
    Register<graphics::WireframeRenderer>(registry, maxEntities, WireframeRendererId, "WireframeRenderer");
    Register<graphics::SkeletalAnimator>(registry, maxEntities, SkeletalAnimatorId, "SkeletalAnimator", ui::editor::SkeletalAnimatorUIWidget, CreateSkeletalAnimator);
    Register<graphics::PointLight>(registry, maxEntities, PointLightId, "PointLight", ui::editor::PointLightUIWidget, CreatePointLight, SerializePointLight, DeserializePointLight);
    Register<graphics::ParticleEmitter>(registry, maxEntities, ParticleEmitterId, "ParticleEmitter", ui::editor::ParticleEmitterUIWidget, CreateParticleEmitter, SerializeParticleEmitter, DeserializeParticleEmitter);
    Register<graphics::SpotLight>(registry, maxEntities, SpotLightId, "SpotLight", ui::editor::SpotLightUIWidget, CreateSpotLight, SerializeSpotLight, DeserializeSpotLight);
}
} // namespace nc
