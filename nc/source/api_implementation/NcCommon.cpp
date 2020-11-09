#include "NcCommon.h"
#include "ecs/ECS.h"
#include "NcDebug.h"
#include "component/Renderer.h"
#include "component/PointLight.h"

namespace nc
{
EntityHandle NcCreateEntity()
{
    return NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityHandle NcCreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag)
{
    V_LOG("Creating entity: " + tag);
    return ecs::ECS::CreateEntity(pos, rot, scale, tag);
}

bool NcDestroyEntity(EntityHandle handle)
{
    V_LOG("Destroying entity: " + std::to_string(handle));
    return ecs::ECS::DestroyEntity(handle);
}

Entity * NcGetEntity(EntityHandle handle)
{
    return nc::ecs::ECS::GetEntity(handle);
}

Entity * NcGetEntity(std::string tag)
{
    return ecs::ECS::GetEntity(std::move(tag));
}

Transform * NcGetTransform(const ComponentHandle transformHandle) noexcept(false)
{
    return ecs::ECS::GetTransformFromComponentHandle(transformHandle);
}

template<> Transform * NcGetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::GetTransformFromEntityHandle(handle);
}

template<> bool nc::NcHasEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::HasComponent<Renderer>(handle);
}

template<> Renderer * nc::NcAddEngineComponent<Renderer>(const EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material) noexcept(false)
{
    return ecs::ECS::AddRenderer(handle, mesh, material);
}

template<> bool nc::NcRemoveEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::RemoveComponent<Renderer>(handle);
}

template<> Renderer * nc::NcGetEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::GetComponent<Renderer>(handle);
}

template<> bool nc::NcHasEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::HasComponent<PointLight>(handle);
}

template<> PointLight * nc::NcAddEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::AddPointLight(handle);
}

template<> bool nc::NcRemoveEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::RemoveComponent<PointLight>(handle);
}

template<> PointLight * nc::NcGetEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return ecs::ECS::GetComponent<PointLight>(handle);
}
} // end namespace nc