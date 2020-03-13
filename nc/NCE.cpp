#include "NCE.h"
#include "Engine.h"

namespace nc{
    
engine::Engine* NCE::m_engine = nullptr;

NCE::NCE(engine::Engine* enginePtr)
{
    NCE::m_engine = enginePtr;
}

graphics::Graphics& NCE::GetGraphics()
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

Transform* NCE::GetTransform(ComponentHandle handle)
{
    return NCE::m_engine->GetTransformPtr(handle);
}

Renderer* NCE::AddRenderer(EntityHandle handle)
{
    return NCE::m_engine->AddRenderer(handle);
}

Renderer* NCE::GetRenderer(EntityHandle handle)
{
    return NCE::m_engine->GetRenderer(handle);
}

bool NCE::RemoveRenderer(EntityHandle handle)
{
    return NCE::m_engine->RemoveRenderer(handle);
}

} //end namespace nc
