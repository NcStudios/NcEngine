#include "NCE.h"
#include "engine/Engine.h"
#include "component/Renderer.h"
#include "component/PointLight.h"

namespace nc{
    
engine::Engine* NCE::m_engine = nullptr;

NCE::NCE(engine::Engine* enginePtr)
{
    m_engine = enginePtr;
}

Transform * NCE::GetMainCameraTransform()
{
    return m_engine->GetMainCameraTransform();
}

void NCE::Exit()
{
    m_engine->Exit();
}

EntityHandle NCE::CreateEntity()
{
    return CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityHandle NCE::CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    return m_engine->CreateEntity(pos, rot, scale, tag);
}

bool NCE::DestroyEntity(EntityHandle handle)
{
    return m_engine->DestroyEntity(handle);
}

Entity* NCE::GetEntity(EntityHandle handle)
{
    return m_engine->GetEntity(handle);
}

Entity* NCE::GetEntity(const std::string& tag)
{
    return m_engine->GetEntity(tag);
}

/*********************
 * Transform Component
 *********************/

/* Public Get */
Transform* NCE::GetTransformFromHandle(const ComponentHandle transformHandle)
{
    return m_engine->GetTransformPtr(transformHandle);
}

Transform * NCE::GetTransformFromEntityHandle(const EntityHandle handle)
{
    auto ptr = GetEntity(handle);
    IF_THROW(!ptr, "NCE::GetTransform - bad handle");
    return m_engine->GetTransformPtr(ptr->Handles.transform);
}

template<> Transform * NCE::GetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    auto ptr = GetEntity(handle);
    IF_THROW(!ptr, "NCE::GetEngineComponent<Transform> - bad handle");
    return m_engine->GetTransformPtr(ptr->Handles.transform);
}

/*********************
 * Renderer Component
 *********************/

/* Public Get/Set */
template<> bool NCE::HasEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    (void)handle;
    return true; //not impl 
}
template<> Renderer * NCE::AddEngineComponent<Renderer>(const EntityHandle handle, graphics::Mesh& mesh) noexcept(false)
{
    return m_engine->AddRenderer(handle, mesh);
}
template<> bool NCE::RemoveEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return m_engine->RemoveRenderer(handle);
}
template<> Renderer * NCE::GetEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return m_engine->GetRenderer(handle);
}

/**********************
 * PointLight Component
 **********************/

/* Public Get/Set */
template<> bool NCE::HasEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    (void)handle;
    return true; //not impl
}
template<> PointLight * NCE::AddEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return m_engine->AddPointLight(handle);
}
template<> bool NCE::RemoveEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return m_engine->RemovePointLight(handle);
}
template<> PointLight * NCE::GetEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return m_engine->GetPointLight(handle);
}

} //end namespace nc
