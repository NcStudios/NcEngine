#ifndef NCE_H
#define NCE_H

#include "Common.h"

#include "External.h"

#include "Win32Process.h"
#include "ProjectConfig.h"

#include "Entity.h"
#include "Time.h"
#include "Input.h"

#include "HandleManager.h"
#include "TransformManager.h"
#include "SceneManager.h"
#include "RenderingSystem.h"
#include "CollisionSystem.h"


class Engine;
class TransformManager;
class CollisionSystem;
class Entity;

class NCE
{
    private:
        static Engine* m_engine;

    public:
        NCE(Engine* enginePtr);

        static EntityHandle CreateEntity();
        static bool DestroyEntity(EntityHandle handle);
        static Entity* GetEntityPtr(EntityHandle handle);
        static Transform* GetTransformPtr(ComponentHandle handle);

};

struct EngineState
{
    bool isRunning = true;
};

struct Entities
{
    std::unordered_map<EntityHandle, Entity> AwaitingInitialize;
    std::unordered_map<EntityHandle, Entity> Active;
    std::unordered_map<EntityHandle, Entity> AwaitingDestroy;
};

class Engine
{
    private:
        EngineState m_engineState;
        Win32Process m_win32Process;
        ProjectConfig m_projectConfig;

        Entities m_entities;
        //std::unordered_map<EntityHandle, Entity> m_entities;

        HandleManager<EntityHandle> m_handleManager;
        std::unique_ptr<RenderingSystem> m_renderingSystem;
        std::unique_ptr<CollisionSystem> m_collisionSystem;
        std::unique_ptr<TransformManager> m_transformManager;

        void FrameUpdate();
        void FixedUpdate();
        void SendOnInitialize();
        void SendFrameUpdate();
        void SendFixedUpdate();
        void SendOnDestroy();

        bool DoesEntityExist(EntityHandle handle);                                             //returns true if entity is in Active or AwaitingInitialize, false otherwise
        std::unordered_map<EntityHandle, Entity>& GetMapContainingEntity(EntityHandle handle, bool checkAll = false); //returns map containing entity, throws exception if not found
        Entity* GetEntityPtrFromAnyMap(EntityHandle handle);                                   //returns ptr to entity regardless of map it's in (AwaitingDestroy, etc.)

    public:
        Engine(Win32Process win32Process, ProjectConfig projectConfig);
        ~Engine();

        void MainLoop();
        void Exit();

        EntityHandle CreateEntity();                        //creates new Entity and Transform and adds it to AwaitingInitialize, returns handle to Entity
        bool DestroyEntity(EntityHandle handle);            //moves entity from current map to AwaitingDestroy, returns true if successful
        Entity* GetEntityPtr(EntityHandle handle);          //returns ptr to entity in Active or AwaitingInitialize maps, returns nullptr if not found
        Transform* GetTransformPtr(ComponentHandle handle); //returns ptr to Transform with given handle, returns nullptr if not found
};

#endif