#ifndef NCE_H
#define NCE_H

#include "Common.h"
#include "NCException.h"
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

namespace nc
{
    class Entity;

    namespace internal { class Engine; class TransformManager; class CollisionSystem; }
    
    class NCE
    {
        public:
            NCE(internal::Engine* enginePtr);

            static EntityHandle CreateEntity();
            static EntityHandle CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics);
            static bool DestroyEntity(EntityHandle handle);
            static Entity* GetEntityPtr(EntityHandle handle);
            static Transform* GetTransformPtr(ComponentHandle handle);
            static ProjectConfig GetProjectConfig() noexcept;
        
        private:
            static internal::Engine* m_engine;
    };

    namespace internal
    {
        class Engine
        {
            public:
                Engine(Win32Process win32Process, ProjectConfig projectConfig);
                ~Engine();

                void MainLoop();
                void Exit();

                EntityHandle CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics); //creates new Entity and Transform and adds it to AwaitingInitialize, returns handle to Entity
                bool DestroyEntity(EntityHandle handle);            //moves entity from current map to AwaitingDestroy, returns true if successful
                Entity* GetEntityPtr(EntityHandle handle);          //returns ptr to entity in Active or AwaitingInitialize maps, returns nullptr if not found
                Transform* GetTransformPtr(ComponentHandle handle); //returns ptr to Transform with given handle, returns nullptr if not found

                ProjectConfig GetProjectConfig() const noexcept;

            private:
                struct EngineState
                {
                    bool isRunning = true;
                } m_engineState;

                struct Subsystem
                {
                    Win32Process Win32;
                    HandleManager<EntityHandle> Handle;
                    std::unique_ptr<RenderingSystem> Rendering;
                    std::unique_ptr<CollisionSystem> Collision;
                    std::unique_ptr<TransformManager> Transform;
                } m_subsystem;

                struct EntityMaps
                {
                    std::unordered_map<EntityHandle, Entity> AwaitingInitialize;
                    std::unordered_map<EntityHandle, Entity> Active;
                    std::unordered_map<EntityHandle, Entity> AwaitingDestroy;
                } m_entities;

                ProjectConfig m_projectConfig;

                void FrameUpdate();
                void FixedUpdate();
                void SendOnInitialize() noexcept;
                void SendFrameUpdate() noexcept;
                void SendFixedUpdate() noexcept;
                void SendOnDestroy() noexcept;

                bool DoesEntityExist(EntityHandle handle);                                                //returns true if entity is in Active or AwaitingInitialize, false otherwise
                auto& GetMapContainingEntity(EntityHandle handle, bool checkAll = false) noexcept(false); //returns map containing entity, throws exception if not found
                Entity* GetEntityPtrFromAnyMap(EntityHandle handle) noexcept(false);                      //returns ptr to entity regardless of map it's in (AwaitingDestroy, etc.)
        };
    } //end namespace internal
} //end namespace nc

#endif