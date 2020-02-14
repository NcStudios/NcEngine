#include "../Include/NCE.h"

/* NCE */
Engine* NCE::m_engine = nullptr;

NCE::NCE(Engine* enginePtr) { NCE::m_engine = enginePtr; }

EntityHandle NCE::CreateEntity() { return NCE::m_engine->CreateEntity(); }

bool NCE::DestroyEntity(EntityHandle handle) { return NCE::m_engine->DestroyEntity(handle); }

Entity* NCE::GetEntityPtr(EntityHandle handle) { return NCE::m_engine->GetEntityPtr(handle); }

Transform* NCE::GetTransformPtr(ComponentHandle handle) { return NCE::m_engine->GetTransformPtr(handle); }
/* end NCE */


/* ENGINE */
Engine::Engine(Win32Process win32Process, ProjectConfig projectConfig) : m_projectConfig(projectConfig)
{
    m_subsystem.Win32 = win32Process;
    m_subsystem.Rendering = std::make_unique<RenderingSystem>(m_subsystem.Win32.CopyBufferToScreen, m_projectConfig.ScreenWidth, m_projectConfig.ScreenHeight);
    m_subsystem.Collision = std::make_unique<CollisionSystem>(this);
    m_subsystem.Transform = std::make_unique<TransformManager>();
}

Engine::~Engine() {}

bool Engine::DoesEntityExist(EntityHandle handle) { return (m_entities.Active.count(handle) > 0) || (m_entities.AwaitingInitialize.count(handle) > 0); }


auto& Engine::GetMapContainingEntity(EntityHandle handle, bool checkAll) noexcept(false)
{
    if (m_entities.Active.count(handle) > 0)
        return m_entities.Active;
    else if (m_entities.AwaitingInitialize.count(handle) > 0)
        return m_entities.AwaitingInitialize;

    if (checkAll && (m_entities.AwaitingDestroy.count(handle) > 0) ) //only check AwaitingDestroy if checkAll flag is set
        return m_entities.AwaitingDestroy;

    throw DefaultException("EntityNotFoundException");
}

Entity* Engine::GetEntityPtrFromAnyMap(EntityHandle handle) noexcept(false)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void Engine::MainLoop()
{
    NCE nce(this);
    Time time;
    SceneManager sceneManager;

    while(m_engineState.isRunning)
    {
        /* CYCLE START */
        time.UpdateTime();
        m_subsystem.Win32.ProcessSystemQueue();

        /* PHYSICS */
        /** @note Change this so physics 'simulates' running at a fixed interval.
         * It may need to run multiple times in a row in cases where FrameUpdate()
         * runs slowly and execution doesn't return back to physics in time for the 
         * next interval.
         */
        if (Time::FixedDeltaTime > m_projectConfig.FixedUpdateInterval)
        {
            FixedUpdate();
            time.ResetFixedDeltaTime();
        }

        /* FRAME */
        if (Time::FrameDeltaTime > m_projectConfig.FrameUpdateInterval)
        {
            FrameUpdate();
            Input::Flush();
            time.ResetFrameDeltaTime();
        }

        /* CYCLE CLEANUP */

    } //end main loop
}

void Engine::FrameUpdate()
{
    // user component init and logic
    SendOnInitialize();
    SendFrameUpdate();

    // rendering
    m_subsystem.Rendering->StartRenderCycle(m_subsystem.Transform->GetVectorOfTransforms());

    // clearup
    SendOnDestroy();
}

void Engine::FixedUpdate()
{
    // user component fixed tick logic
    SendFixedUpdate();

    // check collisions
    m_subsystem.Collision->CheckCollisions(m_subsystem.Transform->GetVectorOfTransforms());
}

void Engine::Exit()
{
    m_engineState.isRunning = false;
}

EntityHandle Engine::CreateEntity()
{
    EntityHandle newHandle = m_subsystem.Handle.GenerateNewHandle();
    Entity newEntity = Entity(newHandle);
    newEntity.TransformHandle = m_subsystem.Transform->Add(newHandle);
    m_entities.AwaitingInitialize.emplace(newHandle, newEntity);
    return newHandle;
}

bool Engine::DestroyEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return false;
    auto& containingMap = GetMapContainingEntity(handle);
    m_entities.AwaitingDestroy.emplace(handle, containingMap.at(handle));
    containingMap.erase(handle);
    return true;
}

Entity* Engine::GetEntityPtr(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = GetMapContainingEntity(handle);
    return &containingMap.at(handle);       
} 

Transform* Engine::GetTransformPtr(ComponentHandle handle) { return m_subsystem.Transform->GetPointerTo(handle); }


void Engine::SendOnInitialize() noexcept
{
    for(auto& pair : m_entities.AwaitingInitialize)
    {
        pair.second.SendOnInitialize();
        m_entities.Active.emplace(std::move(pair));
    }
    m_entities.AwaitingInitialize.clear();
}

void Engine::SendFrameUpdate() noexcept
{
    for(auto& pair : m_entities.Active)
    {
        pair.second.SendFrameUpdate();
    }
}

void Engine::SendFixedUpdate() noexcept
{
    for(auto& pair : m_entities.Active)
    {
        pair.second.SendFixedUpdate();
    }
}

void Engine::SendOnDestroy() noexcept
{
    for(auto& pair : m_entities.AwaitingDestroy)
    {
        Entity* entityPtr = GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            std::cout << "Engine::SendOnDestroy - entityPtr is null\n";
            continue;
        }

        pair.second.SendOnDestroy();
        m_subsystem.Transform->Remove(entityPtr->TransformHandle);
    }
    m_entities.AwaitingDestroy.clear();
}
/* end ENGINE */
