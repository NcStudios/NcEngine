#include "EngineSystems.h"
#include "EngineData.h"
#include "graphics/Graphics.h"
#include "graphics/Mesh.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "scenes/InitialScene.h"
#include "input/Input.h"
#include "ui/UI.h"
#include "NcCamera.h"
#include "NcCommon.h"
#include "NcConfig.h"
#include "NcDebug.h"
#include "NcEngine.h"
#include "NcScene.h"

#include <iostream>
#include <memory>

using namespace nc;

/** Engine State */
namespace
{
    std::unique_ptr<Window> g_WindowInstance{nullptr};
    engine::internal::EngineSystems g_EngineSystems{};
    engine::internal::EngineData g_EngineData{};
}

/** Internal Declarations */
namespace nc::engine::internal
{
    void MainLoop();
    void Exit();
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

/**
 * Interface Impl */
void nc::engine::NcInitializeEngine(HINSTANCE hInstance, const config::detail::ConfigPaths& configPaths)
{
    if (g_WindowInstance)
    {
        throw std::runtime_error("InitializeEngine - engine is already initialized");
    }

    auto config = config::detail::Read(configPaths);
    g_WindowInstance = std::make_unique<Window>(hInstance, config);
    auto dimensions = g_WindowInstance->GetWindowDimensions();
    auto hwnd = g_WindowInstance->GetHWND();
    g_EngineSystems = internal::EngineSystems{ dimensions.first, dimensions.second, hwnd };
    g_EngineData = internal::EngineData{ std::move(config) };

    g_WindowInstance->BindUI(g_EngineSystems.ui.get());
}

void nc::engine::NcStartEngine()
{
    if (!g_WindowInstance)
    {
        throw std::runtime_error("StartEngine - engine is not properly initialized");
    }
    internal::MainLoop();
}

void nc::engine::NcShutdownEngine()
{
    if (!g_WindowInstance)
    {
        throw std::runtime_error("ShutdownEngine - engine is not running");
    }
    internal::Exit();
    g_EngineData = internal::EngineData{};
    g_EngineSystems = internal::EngineSystems{};
    g_WindowInstance = nullptr;
}

void nc::scene::NcChangeScene(std::unique_ptr<scene::Scene>&& scene)
{
    g_EngineData.isSceneSwapScheduled = true;
    g_EngineData.swapScene = std::move(scene);
}

const config::Config& nc::config::NcGetConfigReference()
{
    return g_EngineData.configData;
}

void nc::NcRegisterMainCamera(Camera * camera)
{
    auto handle = camera->GetParentHandle();
    auto entity = NcGetEntity(handle);
    g_EngineData.mainCameraTransform = NcGetTransform(entity->Handles.transform);
    IF_THROW(!g_EngineData.mainCameraTransform, "NcRegisterMainCamera - bad args");
}

Transform * nc::NcGetMainCameraTransform()
{
    return g_EngineData.mainCameraTransform;
}

EntityHandle nc::NcCreateEntity()
{
    return NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "");
}

EntityHandle nc::NcCreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    auto entityHandle = g_EngineSystems.entity->handleManager.GenerateNewHandle();
    auto transHandle = g_EngineSystems.transform->Add(entityHandle, pos, rot, scale);
    g_EngineSystems.entity->GetActiveEntities().emplace(entityHandle, Entity{entityHandle, transHandle, tag} );
    return entityHandle;
}

bool nc::NcDestroyEntity(EntityHandle handle)
{
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
}

void Exit()
{
    g_EngineData.isRunning = false;
}

void ClearAllStateData()
{
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
    g_EngineSystems.ui->BeginFrame();
    g_EngineSystems.rendering->FrameBegin();
    g_EngineSystems.light->BindLights();
    g_EngineSystems.rendering->Frame();
    g_EngineSystems.ui->Frame(&g_EngineData.frameDeltaTimeFactor, g_EngineSystems.frameLogicTimer->Value(), g_EngineSystems.entity->GetActiveEntities());
    g_EngineSystems.ui->EndFrame();
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