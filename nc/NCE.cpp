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

graphics::Graphics* NCE::GetGraphics()
{
    return m_engine->GetGraphics();
}

EntityView* NCE::GetMainCamera()
{
    return m_engine->GetMainCamera();
}

void NCE::Exit()
{
    m_engine->Exit();
}

EntityView NCE::CreateEntity()
{
    return CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityView NCE::CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    return m_engine->CreateEntity(pos, rot, scale, tag);
}

bool NCE::DestroyEntity(EntityHandle handle)
{
    return m_engine->DestroyEntity(handle);
}

EntityView NCE::GetEntityView(EntityHandle handle)
{
    ComponentHandle transHandle = NCE::GetEntity(handle)->Handles.transform;
    return EntityView(handle, transHandle);
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
Transform* NCE::GetTransform(const ComponentHandle transformHandle)
{
    return m_engine->GetTransformPtr(transformHandle);
}

Transform * NCE::GetTransform(const EntityView& view)
{
    return m_engine->GetTransformPtr(view.TransformHandle);
}

template<> Transform * NCE::GetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    return m_engine->GetTransformPtr(GetEntityView(handle).TransformHandle);
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
template<> Renderer * NCE::AddEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return m_engine->AddRenderer(handle);
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
