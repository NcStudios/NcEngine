#include "GraphicsTypes.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/WireframeRenderer.h"

namespace nc
{
void RegisterGraphicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities)
{
    Register<graphics::MeshRenderer>(registry, maxEntities, MeshRendererId, "MeshRenderer", editor::MeshRendererUIWidget, CreateMeshRenderer, SerializeMeshRenderer, DeserializeMeshRenderer);
    Register<graphics::ToonRenderer>(registry, maxEntities, ToonRendererId, "ToonRenderer", editor::ToonRendererUIWidget, CreateToonRenderer, SerializeToonRenderer, DeserializeToonRenderer);
    Register<graphics::WireframeRenderer>(registry, maxEntities, WireframeRendererId, "WireframeRenderer");
    Register<graphics::SkeletalAnimator>(registry, maxEntities, SkeletalAnimatorId, "SkeletalAnimator", editor::SkeletalAnimatorUIWidget, CreateSkeletalAnimator);
    Register<graphics::PointLight>(registry, maxEntities, PointLightId, "PointLight", editor::PointLightUIWidget, CreatePointLight, SerializePointLight, DeserializePointLight);
    Register<graphics::ParticleEmitter>(registry, maxEntities, ParticleEmitterId, "ParticleEmitter", editor::ParticleEmitterUIWidget, CreateParticleEmitter, SerializeParticleEmitter, DeserializeParticleEmitter);
}
} // namespace nc
