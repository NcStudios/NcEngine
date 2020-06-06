#include "Engine.h"
#include "RenderingSystem.h"
#include "LightSystem.h"
#include "CollisionSystem.h"
#include "win32/Window.h"
#include "NCE.h"
#include "ProjectSettings.h"
#include "scene/SceneManager.h"
#include "graphics/Graphics.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "debug/NCException.h"
#include "component/Camera.h"
#include "component/Renderer.h"
#include "component/PointLight.h"

#include <iostream>
#include <unordered_map>

#ifdef NC_DEBUG
#include "utils/editor/EditorManager.h"
#endif

namespace nc::engine
{

struct EntityMaps
{
    /** @todo need entity graphs to support entity hierarchies */
    using EntityMap_t = std::unordered_map<EntityHandle, Entity>;
    EntityMap_t ToInitialize;
    EntityMap_t Active;
    EntityMap_t ToDestroy;
};


Engine::Engine(HWND hwnd)
{
    auto wndDim           = Window::Instance->GetWindowDimensions();
    m_entities            = std::make_unique<EntityMaps>();
    m_subsystem.Rendering = std::make_unique<RenderingSystem>(wndDim.first, wndDim.second, Window::Instance->GetHWND());
    m_subsystem.Light     = std::make_unique<LightSystem>();
    m_subsystem.Collision = std::make_unique<CollisionSystem>();
    m_subsystem.Transform = std::make_unique<ComponentSystem<Transform>>();
    m_subsystem.Handle    = std::make_unique<HandleManager<EntityHandle>>();
    
#ifdef NC_DEBUG
    m_editorManager       = std::make_unique<nc::utils::editor::EditorManager>(hwnd, GetGraphics());
    m_frameLogicTimer     = std::make_unique<nc::time::Timer>();
#endif
}

Engine::~Engine() 
{}

bool Engine::DoesEntityExist(EntityHandle handle)
{
    return (m_entities->Active.count(handle) > 0) || (m_entities->ToInitialize.count(handle) > 0);
}


auto& Engine::GetMapContainingEntity(EntityHandle handle, bool checkAll) noexcept(false)
{
    if (m_entities->Active.count(handle) > 0)
        return m_entities->Active;
    else if (m_entities->ToInitialize.count(handle) > 0)
        return m_entities->ToInitialize;

    if (checkAll && (m_entities->ToDestroy.count(handle) > 0) ) //only check ToDestroy if checkAll flag is set
        return m_entities->ToDestroy;

    throw DefaultException("Engine::GetmapContainingEntity() - Entity not found.");
}

Entity* Engine::GetEntityPtrFromAnyMap(EntityHandle handle) noexcept(false)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void Engine::MainLoop()
{
    NCE nce(this);
    time::Time ncTime;
    scene::SceneManager sceneManager;

    m_mainCameraView = CreateEntity(Vector3(0.0f, 0.0f, -15.0f), Vector3::Zero(), Vector3::Zero(), "MainCamera");

    NCE::AddUserComponent<Camera>(m_mainCameraView.Handle);

    while(m_engineState.isRunning)
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
        if (time::Time::FixedDeltaTime > ProjectSettings::displaySettings.fixedUpdateInterval)
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
        #ifdef NC_DEBUG
        m_frameLogicTimer->Start();
        #endif
        FrameLogic(time::Time::FrameDeltaTime * m_frameDeltaTimeFactor);
        #ifdef NC_DEBUG
        m_frameLogicTimer->Stop();
        #endif
        FrameRender(time::Time::FrameDeltaTime * m_frameDeltaTimeFactor);
        FrameCleanup();
        input::Flush();
        ncTime.ResetFrameDeltaTime();

        /************
        * CYCLE END *
        *************/

    } //end main loop
}

void Engine::FrameLogic(float dt)
{
    SendOnInitialize();
    SendFrameUpdate(dt);
}

void Engine::FrameRender(float dt)
{
    (void)dt;
    #ifdef NC_DEBUG
    m_editorManager->BeginFrame();
    #endif

    m_subsystem.Rendering->FrameBegin();
    m_subsystem.Light->BindLights(GetGraphics());
    m_subsystem.Rendering->Frame();

    #ifdef NC_DEBUG
    m_editorManager->Frame(&m_frameDeltaTimeFactor, m_frameLogicTimer->Value(), GetGraphics()->GetDrawCallCount(), m_entities->Active);
    m_editorManager->EndFrame();
    #endif

    m_subsystem.Rendering->FrameEnd();
}

void Engine::FrameCleanup()
{
    SendOnDestroy();
}

void Engine::FixedUpdate()
{
    // user component fixed tick logic
    SendFixedUpdate();

    // check collisions
    m_subsystem.Collision->CheckCollisions(const_cast<const std::vector<Transform>&>(m_subsystem.Transform->GetVector()));
}

void Engine::Exit()
{
    m_engineState.isRunning = false;
}

EntityView Engine::CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, const std::string& tag)
{
    auto entityHandle = m_subsystem.Handle->GenerateNewHandle();
    auto expectedTransHandle = m_subsystem.Transform->GetCurrentHandle() + 1;
    auto transHandle = m_subsystem.Transform->Add(EntityView { entityHandle, expectedTransHandle } );
    
    auto newEntity = Entity(entityHandle, tag);
    newEntity.Handles.transform = transHandle;
    
    auto transformPtr = GetTransformPtr(newEntity.Handles.transform);
    transformPtr->Set(pos, rot, scale);
    m_entities->ToInitialize.emplace(entityHandle, std::move(newEntity));
    return EntityView { entityHandle, transHandle };
}

bool Engine::DestroyEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return false;
    auto& containingMap = GetMapContainingEntity(handle);
    m_entities->ToDestroy.emplace(handle, std::move(containingMap.at(handle)));
    containingMap.erase(handle);
    return true;
}

Entity* Engine::GetEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = GetMapContainingEntity(handle);
    return &containingMap.at(handle);       
} 

Entity* Engine::GetEntity(const std::string& tag)
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

Renderer* Engine::AddRenderer(EntityHandle handle)
{
    if(GetRenderer(handle))
    {
        return nullptr;
    }

    auto view = EntityView { handle, GetEntity(handle)->Handles.transform };
    GetEntity(handle)->Handles.renderer = m_subsystem.Rendering->Add(view);
    return GetRenderer(handle);
}

Renderer* Engine::GetRenderer(EntityHandle handle)
{
    return m_subsystem.Rendering->GetPointerTo(GetEntity(handle)->Handles.renderer);
}

bool Engine::RemoveRenderer(EntityHandle handle)
{
    return m_subsystem.Rendering->Remove(handle);
}

PointLight* Engine::AddPointLight(EntityHandle handle)
{
    if(GetPointLight(handle))
    {
        return nullptr;
    }

    EntityView view(handle, GetEntity(handle)->Handles.transform);
    GetEntity(handle)->Handles.pointLight = m_subsystem.Light->Add(view);
    GetPointLight(handle)->Set(GetGraphics(), {0.0f, 0.0f, 0.0f});
    return GetPointLight(handle);
}

PointLight* Engine::GetPointLight(EntityHandle handle)
{
    return m_subsystem.Light->GetPointerTo(GetEntity(handle)->Handles.pointLight);
}

bool Engine::RemovePointLight(EntityHandle handle)
{
    return m_subsystem.Light->Remove(handle);
}

nc::graphics::Graphics* Engine::GetGraphics()
{
    return m_subsystem.Rendering->GetGraphics();
}

#ifdef NC_DEBUG
nc::utils::editor::EditorManager* Engine::GetEditorManager()
{
    return m_editorManager.get();
}
#endif

EntityView* Engine::GetMainCamera()
{
    return &m_mainCameraView;
}

Transform* Engine::GetTransformPtr(ComponentHandle handle) { return m_subsystem.Transform->GetPointerTo(handle); }

void Engine::SendOnInitialize() noexcept
{
    for(auto& pair : m_entities->ToInitialize)
    {
        pair.second.SendOnInitialize();
        m_entities->Active.emplace(std::move(pair));
    }
    m_entities->ToInitialize.clear();
}

void Engine::SendFrameUpdate(float dt) noexcept
{
    for(auto& pair : m_entities->Active)
    {
        pair.second.SendFrameUpdate(dt);
    }
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
        m_subsystem.Transform->Remove(entityPtr->Handles.transform);
    }
    m_entities->ToDestroy.clear();
}
}// end namespace nc::internal
