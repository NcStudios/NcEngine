#include "EngineSystems.h"
#include "EngineData.h"
#include "graphics/Graphics.h"
#include "graphics/Mesh.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "log/Logger.h"
#include "scenes/InitialScene.h"
#include "input/Input.h"
#include "NcCamera.h"
#include "NcCommon.h"
#include "NcConfig.h"
#include "NcDebug.h"
#include "NcEngine.h"
#include "NcLog.h"
#include "NcScene.h"
#include "NcUI.h"

#include <iostream>
#include <memory>

using namespace nc;

/** Engine State */
namespace
{
    std::unique_ptr<log::Logger> g_Logger{nullptr};
    std::unique_ptr<Window> g_WindowInstance{nullptr};
    engine::internal::EngineSystems g_EngineSystems{};
    engine::internal::EngineData g_EngineData{};
}

/** Internal Declarations */
namespace nc::engine::internal
{
    void MainLoop();
    void NukeStateData();
    void ClearAllStateData();
    void DoSceneSwap();

    void FrameLogic(float dt);
    void FrameRender(float dt);
    void FixedUpdate();
    void FrameCleanup();
    void SendFrameUpdateToEntities(float dt);
    void SendFixedUpdateToEntities();
    void SendOnDestroyToEntities();
}

#ifdef VERBOSE_LOGGING_ENABLED
    #define V_LOG(item); \
            g_Logger->Log(item); \
            g_Logger->Log(std::string("    Func: ") + __PRETTY_FUNCTION__); \
            g_Logger->Log(std::string("    File: ") + __FILE__); \
            g_Logger->Log(std::string("    Line: ") + std::to_string(__LINE__));
#else
    #define V_LOG(item);
#endif

/**
 * Interface Impl */
void nc::engine::NcInitializeEngine(HINSTANCE hInstance)
{
    if (g_WindowInstance)
    {
        throw std::runtime_error("InitializeEngine - engine is already initialized");
    }

    auto config = config::detail::Load();
    g_Logger = std::make_unique<log::Logger>(config.project.logFilePath);
    g_WindowInstance = std::make_unique<Window>(hInstance, config);
    auto dimensions = g_WindowInstance->GetWindowDimensions();
    auto hwnd = g_WindowInstance->GetHWND();
    g_EngineSystems = internal::EngineSystems{ dimensions.first, dimensions.second, hwnd };
    g_EngineData = internal::EngineData{ std::move(config) };
    g_WindowInstance->BindUISystem(g_EngineSystems.ui.get());

    V_LOG("Engine initialized");
}

void nc::engine::NcStartEngine()
{
    V_LOG("Starting engine");
    if (!g_WindowInstance)
    {
        throw std::runtime_error("StartEngine - engine is not properly initialized");
    }
    internal::MainLoop();
}

void nc::engine::NcShutdownEngine(bool forceImmediate)
{
    V_LOG("Shutting down engine - forceImmediate=" + std::to_string(forceImmediate));

    if (forceImmediate)
    {
        internal::NukeStateData();
    }
    else
    {
        g_EngineData.isRunning = false;
    }
}

void nc::scene::NcChangeScene(std::unique_ptr<scene::Scene>&& scene)
{
    V_LOG("Setting swap scene to - " + std::string(typeid(scene).name()));
    g_EngineData.isSceneSwapScheduled = true;
    g_EngineData.swapScene = std::move(scene);
}

const config::Config& nc::config::NcGetConfigReference()
{
    return g_EngineData.configData;
}

void nc::config::NcSetUserName(std::string name)
{
    g_EngineData.configData.user.userName = std::move(name);
}

void nc::log::NcRegisterGameLog(nc::log::IGameLog* log)
{
    V_LOG("Registering game log");
    g_EngineSystems.gameLog = log;
}

void nc::log::NcLogToGame(std::string item)
{
    if(g_EngineSystems.gameLog == nullptr)
    {
        throw std::runtime_error("NcLogToGame - no game log registered");
    }
    g_EngineSystems.gameLog->Log(std::move(item));
}

void nc::log::NcLogToDiagnostics(std::string item)
{
    g_Logger->Log(std::move(item));
}

void nc::NcRegisterMainCamera(Camera * camera)
{
    V_LOG("Registering main camera");
    auto handle = camera->GetParentHandle();
    auto entity = NcGetEntity(handle);
    g_EngineData.mainCameraTransform = NcGetTransform(entity->Handles.transform);
    IF_THROW(!g_EngineData.mainCameraTransform, "NcRegisterMainCamera - bad args");
}

Transform * nc::NcGetMainCameraTransform()
{
    return g_EngineData.mainCameraTransform;
}

void nc::ui::NcRegisterUI(IUI* ui)
{
    V_LOG("Registering project UI");
    g_EngineSystems.ui->BindProjectUI(ui);
}

EntityHandle nc::NcCreateEntity()
{
    return NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityHandle nc::NcCreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    V_LOG("Creating entity: " + tag);
    auto entityHandle = g_EngineSystems.entity->handleManager.GenerateNewHandle();
    auto transHandle = g_EngineSystems.transform->Add(entityHandle, pos, rot, scale);
    g_EngineSystems.entity->GetActiveEntities().emplace(entityHandle, Entity{entityHandle, transHandle, tag} );
    return entityHandle;
}

bool nc::NcDestroyEntity(EntityHandle handle)
{
    V_LOG("Destroying entity: " + std::to_string(handle));
    if (!g_EngineSystems.entity->DoesEntityExist(handle))
        return false;
    auto& containingMap = g_EngineSystems.entity->GetMapContainingEntity(handle);
    g_EngineSystems.entity->GetToDestroyEntities().emplace(handle, std::move(containingMap.at(handle)));
    containingMap.erase(handle);
    return true;
}

Entity * nc::NcGetEntity(EntityHandle handle)
{
    if (!g_EngineSystems.entity->DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = g_EngineSystems.entity->GetMapContainingEntity(handle);
    return &containingMap.at(handle); 
}

Entity * nc::NcGetEntity(const std::string& tag)
{
    for(auto& pair : g_EngineSystems.entity->GetActiveEntities())
    {
        if(tag == pair.second.Tag)
        {
            return &pair.second;
        }
    }
    return nullptr;
}

Transform * nc::NcGetTransform(const ComponentHandle transformHandle) noexcept(false)
{
    auto exists = g_EngineSystems.transform->Contains(transformHandle);
    IF_THROW(!exists, "NcGetTransform - bad handle");
    return g_EngineSystems.transform->GetPointerTo(transformHandle);
}

template<> Transform * nc::NcGetEngineComponent<Transform>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "GetEngineComponent<Transform> - bad handle");
    return g_EngineSystems.transform->GetPointerTo(entity->Handles.transform);
}

template<> bool nc::NcHasEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcHasEngineComponent<Renderer> - bad handle");
    return g_EngineSystems.light->Contains(entity->Handles.renderer);
}

template<> Renderer * nc::NcAddEngineComponent<Renderer>(const EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcAddEngineComponent<Renderer> - bad handle");
    IF_THROW(g_EngineSystems.rendering->Contains(entity->Handles.renderer), "NcAddEngineComponent<Renderer> - entity already has a renderer");

    auto rendererHandle = g_EngineSystems.rendering->Add(handle, mesh, material);
    entity->Handles.renderer = rendererHandle;
    return g_EngineSystems.rendering->GetPointerTo(rendererHandle);
}

template<> bool nc::NcRemoveEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcRemoveEngineComponent<Renderer> - bad handle");
    return g_EngineSystems.rendering->Remove(entity->Handles.renderer);
}

template<> Renderer * nc::NcGetEngineComponent<Renderer>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcGetEngineComponent<Renderer> - bad handle");
    return g_EngineSystems.rendering->GetPointerTo(entity->Handles.renderer);
}

template<> bool nc::NcHasEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcHasEngineComponent<PointLight> - bad handle");
    return g_EngineSystems.light->Contains(entity->Handles.pointLight);
}

template<> PointLight * nc::NcAddEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcAddEngineComponent<PointLight> - bad handle");
    IF_THROW(g_EngineSystems.light->Contains(entity->Handles.pointLight), "NcAddEngineComponent<PointLight> - entity already has a point light");

    auto lightHandle = g_EngineSystems.light->Add(handle);
    entity->Handles.pointLight = lightHandle;
    auto lightPtr = g_EngineSystems.light->GetPointerTo(lightHandle);
    lightPtr->Set({0.0f, 0.0f, 0.0f});
    return lightPtr;
}

template<> bool nc::NcRemoveEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcRemoveEngineComponent<PointLight> - bad handle");
    return g_EngineSystems.light->Remove(entity->Handles.pointLight);
}

template<> PointLight * nc::NcGetEngineComponent<PointLight>(const EntityHandle handle) noexcept(false)
{
    auto entity = NcGetEntity(handle);
    IF_THROW(!entity, "NcRemoveEngineComponent<PointLight> - bad handle");
    return g_EngineSystems.light->GetPointerTo(entity->Handles.pointLight);
}

// Internal Impl
namespace nc::engine::internal
{
void MainLoop()
{
    V_LOG("Starting engine loop");
    time::Time ncTime;
    g_EngineData.activeScene = std::make_unique<InitialScene>();
    g_EngineData.activeScene->Load();

    while(g_EngineData.isRunning)
    {
        ncTime.UpdateTime();
        g_WindowInstance->ProcessSystemMessages();

        /** @note Change this so physics 'simulates' running at a fixed interval.
         * It may need to run multiple times in a row in cases where FrameUpdate()
         * runs slowly and execution doesn't return back to physics in time for the 
         * next interval.
         */
        if (time::Time::FixedDeltaTime > g_EngineData.configData.physics.fixedUpdateInterval)
        {
            FixedUpdate();
            ncTime.ResetFixedDeltaTime();
        }

        auto dt = time::Time::FrameDeltaTime * g_EngineData.frameDeltaTimeFactor;

        #ifdef NC_EDITOR_ENABLED
        g_EngineSystems.frameLogicTimer->Start();
        #endif
        FrameLogic(dt);
        #ifdef NC_EDITOR_ENABLED
        g_EngineSystems.frameLogicTimer->Stop();
        #endif
        FrameRender(dt);
        FrameCleanup();
        ncTime.ResetFrameDeltaTime();
    } //end main loop

    internal::NukeStateData();
}

void NukeStateData()
{
    V_LOG("Nuking engine state");
    config::detail::Save(g_EngineData.configData);
    g_EngineData = internal::EngineData{};
    g_EngineSystems = internal::EngineSystems{};
    g_WindowInstance = nullptr;
    g_Logger = nullptr;
}

void ClearAllStateData()
{
    V_LOG("Clearing engine state");
    auto handles = std::vector<EntityHandle>{};
    for(const auto& pair : g_EngineSystems.entity->GetActiveEntities())
    {
        handles.emplace_back(pair.first);
    }
    for(const auto handle : handles)
    {
        NcDestroyEntity(handle);
    }
    SendOnDestroyToEntities();
    
    g_EngineSystems.entity->GetActiveEntities().clear();
    g_EngineSystems.entity->GetToDestroyEntities().clear();
    g_EngineSystems.entity->handleManager.Reset();
    g_EngineData.isSceneSwapScheduled = false;
    g_EngineData.activeScene = nullptr;
    //do not clear swap scene
    g_EngineData.mainCameraTransform = nullptr;

    g_EngineSystems.transform->Clear();
    g_EngineSystems.rendering->Clear();
    g_EngineSystems.light->Clear();   
}

void DoSceneSwap()
{
    V_LOG("Swapping scene to - " + std::string(typeid(g_EngineData.swapScene).name()));
    g_EngineData.activeScene->Unload();
    ClearAllStateData();
    g_EngineData.activeScene = std::move(g_EngineData.swapScene);
    g_EngineData.activeScene->Load();
}

void FrameLogic(float dt)
{
    SendFrameUpdateToEntities(dt);
}

void FrameRender(float dt)
{
    (void)dt;
    g_EngineSystems.ui->FrameBegin();
    g_EngineSystems.rendering->FrameBegin();
    g_EngineSystems.light->BindLights();
    g_EngineSystems.rendering->Frame();
    g_EngineSystems.ui->Frame(&g_EngineData.frameDeltaTimeFactor, g_EngineSystems.frameLogicTimer->Value(), g_EngineSystems.entity->GetActiveEntities());
    g_EngineSystems.ui->FrameEnd();
    g_EngineSystems.rendering->FrameEnd();
}

void FixedUpdate()
{
    SendFixedUpdateToEntities();
}

void FrameCleanup()
{
    SendOnDestroyToEntities();
    if(g_EngineData.isSceneSwapScheduled)
    {
        DoSceneSwap();
    }
    input::Flush();
}

void SendFrameUpdateToEntities(float dt)
{
    for(auto & pair : g_EngineSystems.entity->GetActiveEntities())
    {
        pair.second.SendFrameUpdate(dt);
    }
}

void SendFixedUpdateToEntities()
{
    for(auto & pair : g_EngineSystems.entity->GetActiveEntities())
    {
        pair.second.SendFixedUpdate();
    }
}

void SendOnDestroyToEntities()
{
    auto & toDestroy = g_EngineSystems.entity->GetToDestroyEntities();
    for(auto & pair : toDestroy)
    {
        Entity* entityPtr = g_EngineSystems.entity->GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            continue;
        }

        pair.second.SendOnDestroy();
        const auto& handles = entityPtr->Handles;
        g_EngineSystems.transform->Remove(handles.transform);
        g_EngineSystems.rendering->Remove(handles.renderer);
        g_EngineSystems.light->Remove(handles.pointLight);
    }
    toDestroy.clear();
}
}// end namespace nc::engine::internal