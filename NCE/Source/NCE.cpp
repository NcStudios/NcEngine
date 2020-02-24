#include "../include/NCE.h"
#include "../include/Engine.h"

namespace nc{
    
internal::Engine* NCE::m_engine = nullptr;

NCE::NCE(internal::Engine* enginePtr)
{
    NCE::m_engine = enginePtr;
}

void NCE::Exit()
{
    NCE::m_engine->Exit();
}

EntityHandle NCE::CreateEntity()
{
    return NCE::CreateEntity(Vector4::Zero(), false, false, "");
}

EntityHandle NCE::CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics, const std::string& tag)
{
    return NCE::m_engine->CreateEntity(rect, enableRendering, enablePhysics, tag);
}

bool NCE::DestroyEntity(EntityHandle handle)
{
    return NCE::m_engine->DestroyEntity(handle);
}

Entity* NCE::GetEntity(EntityHandle handle)
{
    return NCE::m_engine->GetEntity(handle);
}

Entity* NCE::GetEntity(const std::string& tag)
{
    return NCE::m_engine->GetEntity(tag);
}

Transform* NCE::GetTransformPtr(ComponentHandle handle)
{
    return NCE::m_engine->GetTransformPtr(handle);
}

} //end namespace nc
