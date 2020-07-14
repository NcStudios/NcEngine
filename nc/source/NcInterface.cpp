#include "NcAll.h"
#include "win32/NCWin32.h"
#include "engine/Engine.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "config/Config.h"
#include "win32/Window.h"

#include <memory>

namespace
{
    std::unique_ptr<nc::engine::Engine> g_EngineInstance = nullptr;
    std::unique_ptr<nc::Window> g_WindowInstance = nullptr;
}

namespace nc
{
namespace engine
{
void NcInitializeEngine(HINSTANCE hInstance, const config::detail::ConfigPaths& configPaths)
{
    if (g_EngineInstance || g_WindowInstance)
    {
        throw NcException("InitializeEngine - engine is already initialized");
    }

    auto config = nc::config::detail::Read(configPaths);
    g_WindowInstance = std::make_unique<nc::Window>(hInstance, config);
    g_EngineInstance = std::make_unique<Engine>(std::move(config));

    #ifdef NC_EDITOR_ENABLED
    g_WindowInstance->BindEditorManager(g_EngineInstance->GetEditorManager());
    #endif
}

void NcStartEngine()
{
    if (!g_EngineInstance || !g_WindowInstance)
    {
        throw NcException("StartEngine - engine is not properly initialized");
    }
    g_EngineInstance->MainLoop();
}

void NcShutdownEngine()
{
    if (!g_EngineInstance || !g_WindowInstance)
    {
        throw NcException("ShutdownEngine - engine is not running");
    }
    g_EngineInstance->Exit();
    g_EngineInstance = nullptr;
    g_WindowInstance = nullptr;
}
} // end namespace engine

namespace scene
{
void NcChangeScene(std::unique_ptr<scene::Scene>&& scene)
{
    g_EngineInstance->ChangeScene(std::move(scene));
}
} // end namespace scene

namespace config
{
const config::Config& NcGetConfigReference()
{
    return g_EngineInstance->GetConfigReference();
}
} // end namespace config

void NcRegisterMainCamera(Camera * camera)
{
    g_EngineInstance->RegisterMainCamera(camera);
}

Transform * NcGetMainCameraTransform()
{
    return g_EngineInstance->GetMainCameraTransform();
}

EntityHandle NcCreateEntity()
{
    return NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityHandle NcCreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    return g_EngineInstance->CreateEntity(pos, rot, scale, tag);
}

bool NcDestroyEntity(EntityHandle handle)
{
    return g_EngineInstance->DestroyEntity(handle);
}

Entity* NcGetEntity(EntityHandle handle)
{
    return g_EngineInstance->GetEntity(handle);
}

Entity* NcGetEntity(const std::string& tag)
{
    return g_EngineInstance->GetEntity(tag);
}

/*********************
 * Transform Component
 *********************/

/* Public Get */
Transform* NcGetTransformFromHandle(const ComponentHandle transformHandle)
{
    return g_EngineInstance->GetTransformPtr(transformHandle);
}

Transform * NcGetTransformFromEntityHandle(const EntityHandle handle)
{
    auto ptr = NcGetEntity(handle);
    IF_THROW(!ptr, "GetTransform - bad handle");
    return g_EngineInstance->GetTransformPtr(ptr->Handles.transform);
}

template<> Transform * NcGetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    auto ptr = NcGetEntity(handle);
    IF_THROW(!ptr, "GetEngineComponent<Transform> - bad handle");
    return g_EngineInstance->GetTransformPtr(ptr->Handles.transform);
}

/*********************
 * Renderer Component
 *********************/

/** @todo has not implemented */
template<> bool NcHasEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    (void)handle;
    return true;
}
template<> Renderer * NcAddEngineComponent<Renderer>(const EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material) noexcept(false)
{
    return g_EngineInstance->AddRenderer(handle, mesh, material);
}
template<> bool NcRemoveEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return g_EngineInstance->RemoveRenderer(handle);
}
template<> Renderer * NcGetEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    return g_EngineInstance->GetRenderer(handle);
}

/**********************
 * PointLight Component
 **********************/

/** @todo has not implemented */
template<> bool NcHasEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    (void)handle;
    return true;
}
template<> PointLight * NcAddEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return g_EngineInstance->AddPointLight(handle);
}
template<> bool NcRemoveEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return g_EngineInstance->RemovePointLight(handle);
}
template<> PointLight * NcGetEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    return g_EngineInstance->GetPointLight(handle);
}

} //end namespace nc
