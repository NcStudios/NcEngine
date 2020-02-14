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
Engine::Engine(Win32Process win32Process, ProjectConfig projectConfig) 
      : m_win32Process(win32Process), m_projectConfig(projectConfig)
{
    m_renderingSystem = std::make_unique<RenderingSystem>(m_win32Process.CopyBufferToScreen, m_projectConfig.ScreenWidth, m_projectConfig.ScreenHeight);
    m_collisionSystem = std::make_unique<CollisionSystem>(this);
    m_transformManager = std::make_unique<TransformManager>();
}

Engine::~Engine() {}

bool Engine::DoesEntityExist(EntityHandle handle) { return (m_entities.Active.count(handle) > 0) || (m_entities.AwaitingInitialize.count(handle) > 0); }

class DefaultException : public std::exception
{
    private:
        const char* message;

    public:
        DefaultException(const char* msg) : message(msg) {}

        const char* what() const throw()
        {
            return message;
        }
};

std::unordered_map<EntityHandle, Entity>& Engine::GetMapContainingEntity(EntityHandle handle, bool checkAll)
{
    if (m_entities.Active.count(handle) > 0)
        return m_entities.Active;
    else if (m_entities.AwaitingInitialize.count(handle) > 0)
        return m_entities.AwaitingInitialize;

    if (checkAll && (m_entities.AwaitingDestroy.count(handle) > 0) ) //only check AwaitingDestroy if checkAll flag is set
        return m_entities.AwaitingDestroy;

    std::cout << "uh oh" << std::endl;
    throw DefaultException("EntityNotFoundException");
}

Entity* Engine::GetEntityPtrFromAnyMap(EntityHandle handle) { return &GetMapContainingEntity(handle, true).at(handle); }

void Engine::MainLoop()
{
    NCE nce(this);
    Time time;
    SceneManager sceneManager;

    while(m_engineState.isRunning)
    {
        /* CYCLE START */
        time.UpdateTime();
        m_win32Process.ProcessSystemQueue();

        /* PHYSICS */
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
    SendOnInitialize();

    /* LOGIC */
    SendFrameUpdate();

    /* RENDERING */
    m_renderingSystem->StartRenderCycle(m_transformManager->GetVectorOfTransforms());

    /* CLEANUP */
    SendOnDestroy();
}

void Engine::FixedUpdate()
{
    /* LOGIC */
    SendFixedUpdate();
    /* COLLISIONS */
    m_collisionSystem->CheckCollisions(m_transformManager->GetVectorOfTransforms());
}

void Engine::Exit()
{
    m_engineState.isRunning = false;
}

EntityHandle Engine::CreateEntity()
{
    EntityHandle newHandle = m_handleManager.GenerateNewHandle();
    Entity newEntity = Entity(newHandle);
    newEntity.TransformHandle = m_transformManager->Add(newHandle);
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

Transform* Engine::GetTransformPtr(ComponentHandle handle) { return m_transformManager->GetPointerTo(handle); }


void Engine::SendOnInitialize()
{
    for(auto& pair : m_entities.AwaitingInitialize)
    {
        pair.second.SendOnInitialize();
        m_entities.Active.emplace(std::move(pair));
    }
    m_entities.AwaitingInitialize.clear();
}

void Engine::SendFrameUpdate()
{
    for(auto& pair : m_entities.Active)
    {
        pair.second.SendFrameUpdate();
    }
}

void Engine::SendFixedUpdate()
{
    for(auto& pair : m_entities.Active)
    {
        pair.second.SendFixedUpdate();
    }
}

void Engine::SendOnDestroy()
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
        m_transformManager->Remove(entityPtr->TransformHandle);
    }
    m_entities.AwaitingDestroy.clear();
}
    /* end ENGINE */
