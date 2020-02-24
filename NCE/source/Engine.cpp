#include <windows.h>
#include "../include/Engine.h"
#include "../include/NCE.h"
#include "../include/Window.h"
#include "../include/TransformSystem.h"
#include "../include/SceneManager.h"
#include "../include/RenderingSystem.h"
#include "../include/CollisionSystem.h"
#include "../include/HandleManager.h"

namespace nc::internal
{
Engine::Engine()//Win32Process win32Process)
{
    //m_subsystem.Win32 = win32Process;
    //m_subsystem.Rendering = std::make_unique<RenderingSystem>(m_subsystem.Win32.CopyBufferToScreen, ProjectSettings::displaySettings.screenWidth, ProjectSettings::displaySettings.screenHeight);
    m_subsystem.Rendering = std::make_unique<RenderingSystem>(Window::CopyBufferToScreen, ProjectSettings::displaySettings.screenWidth, ProjectSettings::displaySettings.screenHeight);
    m_subsystem.Collision = std::make_unique<CollisionSystem>();
    m_subsystem.Transform = std::make_unique<TransformSystem>();
    m_subsystem.Handle    = std::make_unique<HandleManager<EntityHandle>>();
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
    time::Time ncTime;
    scene::SceneManager sceneManager;

    while(m_engineState.isRunning)
    {
        /* CYCLE START */
        ncTime.UpdateTime();
        //m_subsystem.Win32.ProcessSystemQueue();
        Window::ProcessSystemMessages();
        /* PHYSICS */
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

        /* FRAME */
        // if (time::Time::FrameDeltaTime > ProjectSettings::displaySettings.frameUpdateInterval)
        // {
        //     FrameUpdate();
        //     input::Flush();
        //     ncTime.ResetFrameDeltaTime();
        // }
        FrameUpdate();
        input::Flush();
        ncTime.ResetFrameDeltaTime();

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

    // cleanup
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

EntityHandle Engine::CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics, const std::string& tag)
{
    EntityHandle newHandle = m_subsystem.Handle->GenerateNewHandle();
    Entity newEntity = Entity(newHandle, tag);
    newEntity.TransformHandle = m_subsystem.Transform->Add(newHandle);
    Transform* transformPtr = GetTransformPtr(newEntity.TransformHandle);
    transformPtr->SetRect(rect);
    transformPtr->ToggleRenderingEnabled(enableRendering);
    transformPtr->TogglePhysicsEnabled(enablePhysics);
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

Entity* Engine::GetEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = GetMapContainingEntity(handle);
    return &containingMap.at(handle);       
} 

Entity* Engine::GetEntity(const std::string& tag)
{
    for(auto& pair : m_entities.Active)
    {
        if(tag == pair.second.Tag)
        {
            return &pair.second;
        }
    }
    return nullptr;
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
}// end namespace nc::internal
