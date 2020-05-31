#include "NCE.h"
#include "Engine.h"
#include "Renderer.h"
#include "PointLight.h"

namespace nc{
    
engine::Engine* NCE::m_engine = nullptr;

NCE::NCE(engine::Engine* enginePtr)
{
    NCE::m_engine = enginePtr;
}

graphics::Graphics* NCE::GetGraphics()
{
    return NCE::m_engine->GetGraphics();
}

EntityView* NCE::GetMainCamera()
{
    return NCE::m_engine->GetMainCamera();
}

void NCE::Exit()
{
    NCE::m_engine->Exit();
}

EntityView NCE::CreateEntity()
{
    return NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityView NCE::CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    return NCE::m_engine->CreateEntity(pos, rot, scale, tag);
}

bool NCE::DestroyEntity(EntityHandle handle)
{
    return NCE::m_engine->DestroyEntity(handle);
}

EntityView NCE::GetEntityView(EntityHandle handle)
{
    ComponentHandle transHandle = NCE::GetEntity(handle)->TransformHandle;
    return EntityView(handle, transHandle);
}

Entity* NCE::GetEntity(EntityHandle handle)
{
    return NCE::m_engine->GetEntity(handle);
}

Entity* NCE::GetEntity(const std::string& tag)
{
    return NCE::m_engine->GetEntity(tag);
}

/*********************
 * Transform Component
 *********************/

/* Private */

/* Public Get */
Transform* NCE::GetTransform(const ComponentHandle transformHandle)
{
    return NCE::m_engine->GetTransformPtr(transformHandle);
}

Transform * NCE::GetTransform(const EntityView& view)
{
    return NCE::m_engine->GetTransformPtr(view.TransformHandle);
}

template<> Transform * NCE::GetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    EntityView ev = NCE::GetEntityView(handle);
    return GetTransform(ev.TransformHandle);
}

/*********************
 * Renderer Component
 *********************/

/* Private Get/Set */
Renderer* NCE::AddRenderer(EntityHandle handle) { return NCE::m_engine->AddRenderer(handle);}
bool NCE::RemoveRenderer(EntityHandle handle)   { return NCE::m_engine->RemoveRenderer(handle);}
Renderer* NCE::GetRenderer(EntityHandle handle) { return NCE::m_engine->GetRenderer(handle);}

/* Public Get/Set */
template<> bool NCE::HasEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return true; //not impl 
}
template<> Renderer * NCE::AddEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return AddRenderer(handle);
}
template<> bool NCE::RemoveEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return NCE::RemoveRenderer(handle);
}
template<> Renderer * NCE::GetEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return NCE::GetRenderer(handle);
}

/**********************
 * PointLight Component
 **********************/

/* Private Get/Set */
PointLight* NCE::AddPointLight(EntityHandle handle) { return NCE::m_engine->AddPointLight(handle); }
bool NCE::RemovePointLight(EntityHandle handle) { return NCE::m_engine->RemovePointLight(handle); }
PointLight* NCE::GetPointLight(EntityHandle handle) { return NCE::m_engine->GetPointLight(handle); }

/* Public Get/Set */
template<> bool NCE::HasEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return true; //not impl
}
template<> PointLight * NCE::AddEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return AddPointLight(handle);
}
template<> bool NCE::RemoveEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return RemovePointLight(handle);
}
template<> PointLight * NCE::GetEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return GetPointLight(handle);
}


} //end namespace nc
