#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "NCWinDef.h"
#include "Common.h"
#include "EntityView.h"

//forward declarations
namespace nc
{
    class Camera;
    class Entity;
    class Renderer;
    class Transform;
    class Vector3;
    class Vector4;

    namespace utils::editor { class EditorManager; }
    namespace graphics      { class Graphics; }

    namespace engine { struct EntityMaps;
                       class CollisionSystem;
                       class RenderingSystem;
                       template<class T> class HandleManager;
                       template<class T> class ComponentManager; }
}

namespace nc::engine
{
    class Engine
    {
        public:
            Engine(HWND hwnd);
            ~Engine();

            void MainLoop();
            void Exit();

            EntityView CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, const std::string& tag); //creates new Entity and Transform and adds it to AwaitingInitialize, returns handle to Entity
            bool       DestroyEntity(EntityHandle handle);   //moves entity from current map to AwaitingDestroy, returns true if successful
            Transform* GetTransformPtr(ComponentHandle handle); //returns ptr to Transform with given handle, returns nullptr if not found
            Entity*    GetEntity(EntityHandle handle);    //returns ptr to entity in Active or AwaitingInitialize maps, returns nullptr if not found
            Entity*    GetEntity(const std::string& tag); //returns pointer to first active found entity with tag or nullptr if not found

            Renderer* AddRenderer(EntityHandle handle);
            Renderer* GetRenderer(EntityHandle handle);
            bool      RemoveRenderer(EntityHandle handle);

            EntityView*                       GetMainCamera();
            nc::graphics::Graphics&           GetGraphics();
            nc::utils::editor::EditorManager* GetEditorManager();

        private:
            struct EngineState { bool isRunning = true; } m_engineState;

            float m_frameDeltaTimeFactor = 1.0f; //for slowing/pausing in debug editor

            struct Subsystem
            {
                std::unique_ptr<HandleManager<EntityHandle>> Handle;
                std::unique_ptr<RenderingSystem> Rendering;
                std::unique_ptr<CollisionSystem> Collision;
                std::unique_ptr<ComponentManager<Transform>> TransformSystem;
            } m_subsystem;

            std::unique_ptr<EntityMaps> m_entities;

            std::unique_ptr<utils::editor::EditorManager> m_editorManager;
            EntityView m_mainCameraView;

            void FrameUpdate(float dt);
            void FixedUpdate();
            void SendOnInitialize()        noexcept;
            void SendFrameUpdate(float dt) noexcept;
            void SendFixedUpdate()         noexcept;
            void SendOnDestroy()           noexcept;

            bool    DoesEntityExist(EntityHandle handle);                                               //returns true if entity is in Active or AwaitingInitialize, false otherwise
            auto&   GetMapContainingEntity(EntityHandle handle, bool checkAll = false) noexcept(false); //returns map containing entity, throws exception if not found
            Entity* GetEntityPtrFromAnyMap(EntityHandle handle)                        noexcept(false); //returns ptr to entity regardless of map it's in (AwaitingDestroy, etc.)
    };
} // end namespace nc::internal
