#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <unordered_map>
#include <string>

#include "Common.h"
#include "NCException.h"
#include "Win32Process.h"
#include "ProjectSettings.h"
#include "Vector.h"
#include "Entity.h"

namespace nc::internal
{
    class TransformSystem;
    class CollisionSystem;
    class RenderingSystem;
    template<class T>
    class HandleManager;

    class Engine
    {
        public:
            Engine(Win32Process win32Process);
            ~Engine();

            void MainLoop();
            void Exit();

            EntityHandle CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics, const std::string& tag); //creates new Entity and Transform and adds it to AwaitingInitialize, returns handle to Entity
            bool DestroyEntity(EntityHandle handle);   //moves entity from current map to AwaitingDestroy, returns true if successful
            Entity* GetEntity(EntityHandle handle);    //returns ptr to entity in Active or AwaitingInitialize maps, returns nullptr if not found
            Entity* GetEntity(const std::string& tag); //returns pointer to first active found entity with tag or nullptr if not found
            Transform* GetTransformPtr(ComponentHandle handle); //returns ptr to Transform with given handle, returns nullptr if not found
            

        private:
            struct EngineState
            {
                bool isRunning = true;
            } m_engineState;

            struct Subsystem
            {
                Win32Process Win32;
                std::unique_ptr<HandleManager<EntityHandle>> Handle;
                std::unique_ptr<RenderingSystem> Rendering;
                std::unique_ptr<CollisionSystem> Collision;
                std::unique_ptr<TransformSystem> Transform;
            } m_subsystem;

            struct EntityMaps
            {
                std::unordered_map<EntityHandle, Entity> AwaitingInitialize;
                std::unordered_map<EntityHandle, Entity> Active;
                std::unordered_map<EntityHandle, Entity> AwaitingDestroy;
            } m_entities;

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
}// end namespace nc::internal

#endif