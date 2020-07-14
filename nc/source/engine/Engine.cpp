#include "Engine.h"
#include "TransformSystem.h"
#include "RenderingSystem.h"
#include "LightSystem.h"
#include "CollisionSystem.h"
#include "win32/Window.h"
#include "config/Config.h"
#include "graphics/Graphics.h"
#include "graphics/Mesh.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "NcDebug.h"
#include "component/Camera.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "scene/Scene.h"
#include "scenes/InitialScene.h"
#include "time/NcTime.h"
#include "input/Input.h"

#include <iostream>
#include <unordered_map>

#ifdef NC_EDITOR_ENABLED
#include "utils/editor/EditorManager.h"
#endif

namespace nc::engine
{

struct EntityMaps
{
    /** @todo need entity graphs to support entity hierarchies */
    using EntityMap_t = std::unordered_map<EntityHandle, Entity>;
    EntityMap_t Active;
    EntityMap_t ToDestroy;
};

Engine::Engine(config::Config config )
    : m_mainCameraTransform{ nullptr },
      m_configData{ std::move(config) },
      m_isRunning{ true },
      m_isSceneSwapScheduled{ false },
      m_activeScene{ nullptr },
      m_swapScene{ nullptr },
      m_frameDeltaTimeFactor{ 1.0f }
{
    Window * wndInst = Window::Instance;
    auto wndDim = wndInst->GetWindowDimensions();
    m_renderingSystem = std::make_unique<RenderingSystem>(wndDim.first, wndDim.second, wndInst->GetHWND());
    m_entities = std::make_unique<EntityMaps>();
    m_handleManager = std::make_unique<HandleManager<EntityHandle>>();
    m_transformSystem = std::make_unique<ComponentSystem<Transform>>();
    m_lightSystem = std::make_unique<LightSystem>();
    m_collisionSystem = std::make_unique<CollisionSystem>();

#ifdef NC_EDITOR_ENABLED
    m_editorManager = std::make_unique<nc::utils::editor::EditorManager>(wndInst->GetHWND(), m_renderingSystem->GetGraphics());
    m_frameLogicTimer = std::make_unique<nc::time::Timer>();
#endif
}

Engine::~Engine() 
{}

void Engine::ChangeScene(std::unique_ptr<scene::Scene>&& scene)
{
    m_isSceneSwapScheduled = true;
    m_swapScene = std::move(scene);
}

void Engine::DoSceneSwap()
{
    m_activeScene->Unload();
    ClearAllStateData();
    m_activeScene = std::move(m_swapScene);
    m_activeScene->Load();
}

void Engine::ClearAllStateData()
{
    m_isSceneSwapScheduled = false;

    auto handles = std::vector<EntityHandle>{};
    for(const auto& pair : m_entities->Active)
    {
        handles.emplace_back(pair.first);
    }
    for(const auto handle : handles)
    {
        DestroyEntity(handle);
    }
    SendOnDestroy();
    
    m_entities->Active.clear();
    m_entities->ToDestroy.clear();
    m_handleManager->Reset();
    m_activeScene = nullptr;
    //do not clear swap scene
    m_mainCameraTransform = nullptr;

    m_transformSystem->Clear();
    m_renderingSystem->Clear();
    m_lightSystem->Clear();   
}

bool Engine::DoesEntityExist(const EntityHandle handle) const
{
    return m_entities->Active.count(handle) > 0;
}


auto& Engine::GetMapContainingEntity(const EntityHandle handle, bool checkAll) const noexcept(false)
{
    if (m_entities->Active.count(handle) > 0)
        return m_entities->Active;

    if (checkAll && (m_entities->ToDestroy.count(handle) > 0) ) //only check ToDestroy if checkAll flag is set
        return m_entities->ToDestroy;

    throw NcException("Engine::GetmapContainingEntity() - Entity not found.");
}

Entity* Engine::GetEntityPtrFromAnyMap(const EntityHandle handle) const noexcept(false)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void Engine::MainLoop()
{
    time::Time ncTime;

    m_activeScene = std::make_unique<InitialScene>();
    m_activeScene->Load();

    while(m_isRunning)
    {   
        /**************
        * CYCLE START *
        ***************/    
        ncTime.UpdateTime();
        Window::Instance->ProcessSystemMessages();

        /**********
        * PHYSICS *
        ***********/
        /** @note Change this so physics 'simulates' running at a fixed interval.
         * It may need to run multiple times in a row in cases where FrameUpdate()
         * runs slowly and execution doesn't return back to physics in time for the 
         * next interval.
         */
        if (time::Time::FixedDeltaTime > m_configData.physics.fixedUpdateInterval)
        {
            FixedUpdate();
            ncTime.ResetFixedDeltaTime();
        }

        /********
        * FRAME *
        *********/
        // if (time::Time::FrameDeltaTime > ProjectSettings::displaySettings.frameUpdateInterval)
        // {
        //     FrameUpdate();
        //     input::Flush();
        //     ncTime.ResetFrameDeltaTime();
        // }
        #ifdef NC_EDITOR_ENABLED
        m_frameLogicTimer->Start();
        #endif
        FrameLogic(time::Time::FrameDeltaTime * m_frameDeltaTimeFactor);
        #ifdef NC_EDITOR_ENABLED
        m_frameLogicTimer->Stop();
        #endif
        FrameRender(time::Time::FrameDeltaTime * m_frameDeltaTimeFactor);
        FrameCleanup();
        ncTime.ResetFrameDeltaTime();

        /************
        * CYCLE END *
        *************/

    } //end main loop
}

void Engine::FrameLogic(float dt)
{
    SendFrameUpdate(dt);
}

void Engine::FrameRender(float dt)
{
    (void)dt;
    #ifdef NC_EDITOR_ENABLED
    m_editorManager->BeginFrame();
    #endif

    m_renderingSystem->FrameBegin();
    m_lightSystem->BindLights();
    m_renderingSystem->Frame();

    #ifdef NC_EDITOR_ENABLED
    m_editorManager->Frame(&m_frameDeltaTimeFactor, m_frameLogicTimer->Value(), m_entities->Active);
    m_editorManager->EndFrame();
    #endif

    m_renderingSystem->FrameEnd();
}

void Engine::FrameCleanup()
{
    SendOnDestroy();
    if(m_isSceneSwapScheduled)
    {
        DoSceneSwap();
    }
    input::Flush();
}

void Engine::FixedUpdate()
{
    // user component fixed tick logic
    SendFixedUpdate();
}

void Engine::Exit()
{
    m_isRunning = false;
}

EntityHandle Engine::CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag)
{
    auto entityHandle = m_handleManager->GenerateNewHandle();
    auto transHandle = m_transformSystem->Add(entityHandle, pos, rot, scale);
    m_entities->Active.emplace(entityHandle, Entity{entityHandle, transHandle, tag} );
    return entityHandle;
}

bool Engine::DestroyEntity(const EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return false;
    auto& containingMap = GetMapContainingEntity(handle);
    m_entities->ToDestroy.emplace(handle, std::move(containingMap.at(handle)));
    containingMap.erase(handle);
    return true;
}

Entity* Engine::GetEntity(const EntityHandle handle) const
{
    if (!DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = GetMapContainingEntity(handle);
    return &containingMap.at(handle);       
} 

Entity* Engine::GetEntity(const std::string& tag) const
{
    for(auto& pair : m_entities->Active)
    {
        if(tag == pair.second.Tag)
        {
            return &pair.second;
        }
    }
    return nullptr;
}

Renderer* Engine::AddRenderer(const EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material)
{
    if(GetRenderer(handle))
    {
        return nullptr;
    }

    GetEntity(handle)->Handles.renderer = m_renderingSystem->Add(handle, mesh, material);
    return GetRenderer(handle);
}

Renderer* Engine::GetRenderer(const EntityHandle handle) const
{
    return m_renderingSystem->GetPointerTo(GetEntity(handle)->Handles.renderer);
}

bool Engine::RemoveRenderer(const EntityHandle handle)
{
    return m_renderingSystem->Remove(GetEntity(handle)->Handles.renderer);
}

PointLight* Engine::AddPointLight(const EntityHandle handle)
{
    if(GetPointLight(handle))
    {
        return nullptr;
    }

    GetEntity(handle)->Handles.pointLight = m_lightSystem->Add(handle);
    GetPointLight(handle)->Set({0.0f, 0.0f, 0.0f});
    return GetPointLight(handle);
}

PointLight* Engine::GetPointLight(const EntityHandle handle) const
{
    return m_lightSystem->GetPointerTo(GetEntity(handle)->Handles.pointLight);
}

bool Engine::RemovePointLight(const EntityHandle handle)
{
    return m_lightSystem->Remove(handle);
}

#ifdef NC_EDITOR_ENABLED
nc::utils::editor::EditorManager* Engine::GetEditorManager()
{
    return m_editorManager.get();
}
#endif

void Engine::RegisterMainCamera(Camera * camera)
{
    auto handle = camera->GetParentHandle();
    auto entity = GetEntity(handle);
    m_mainCameraTransform = GetTransformPtr(entity->Handles.transform);
    IF_THROW(!m_mainCameraTransform, "Engine::RegisterMainCamera - bad args");
}

Transform * Engine::GetMainCameraTransform()
{
    return m_mainCameraTransform;
}

Transform* Engine::GetTransformPtr(const ComponentHandle handle) { return m_transformSystem->GetPointerTo(handle); }

void Engine::SendFrameUpdate(float dt) noexcept
{
    for(auto& pair : m_entities->Active)
    {
        pair.second.SendFrameUpdate(dt);
    }
}

const config::Config& Engine::GetConfigReference() const
{
    return m_configData;
}

void Engine::SendFixedUpdate() noexcept
{
    for(auto& pair : m_entities->Active)
    {
        pair.second.SendFixedUpdate();
    }
}

void Engine::SendOnDestroy() noexcept
{
    for(auto& pair : m_entities->ToDestroy)
    {
        Entity* entityPtr = GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            continue;
        }

        pair.second.SendOnDestroy();
        const auto& handles = entityPtr->Handles;
        m_transformSystem->Remove(handles.transform);
        m_renderingSystem->Remove(handles.renderer);
        m_lightSystem->Remove(handles.pointLight);
    }
    m_entities->ToDestroy.clear();
}
}// end namespace nc::engine
