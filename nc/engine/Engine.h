#pragma once

#include "win32/NCWinDef.h"
#include "Common.h"
#include "config/Config.h"

#include <memory>
#include <string>

//forward declarations
namespace nc
{
    class Camera;
    class Entity;
    class Renderer;
    class PointLight;
    class Transform;
    class Vector3;
    class Vector4;

    namespace time     { class Timer; }
    namespace graphics { class Graphics; class Mesh; }

    namespace engine { struct EntityMaps;
                       class CollisionSystem;
                       class RenderingSystem;
                       class LightSystem;
                       template<class T> class HandleManager;
                       template<class T> class ComponentSystem; }

    #ifdef NC_EDITOR_ENABLED
    namespace utils::editor { class EditorManager; }
    #endif
}

namespace nc::engine
{
    class Engine
    {
        public:
            Engine(config::Config config);
            ~Engine();

            void MainLoop();
            void Exit();

            //creates new Entity and Transform and adds it to ToInitialize, returns handle to Entity
            EntityHandle CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag);

            //moves entity from current map to ToDestroy, returns true if successful
            bool DestroyEntity(const EntityHandle handle);
            
            //returns ptr to Transform with given handle, returns nullptr if not found
            Transform * GetTransformPtr(const ComponentHandle handle);
            
            //returns ptr to entity in Active or ToInitialize maps, returns nullptr if not found
            Entity * GetEntity(const EntityHandle handle) const;
            
            //returns pointer to first active found entity with tag or nullptr if not found
            Entity * GetEntity(const std::string& tag) const;

            Renderer * AddRenderer(const EntityHandle handle, graphics::Mesh& mesh);
            Renderer * GetRenderer(const EntityHandle handle) const;
            bool RemoveRenderer(const EntityHandle handle);

            PointLight * AddPointLight(const EntityHandle handle);
            PointLight * GetPointLight(const EntityHandle handle) const;
            bool RemovePointLight(const EntityHandle handle);

            Transform * GetMainCameraTransform();
            
            #ifdef NC_EDITOR_ENABLED
            nc::utils::editor::EditorManager* GetEditorManager();
            #endif

        private:
            std::unique_ptr<EntityMaps> m_entities;
            std::unique_ptr<HandleManager<EntityHandle>> m_handleManager;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
            std::unique_ptr<RenderingSystem> m_renderingSystem;
            std::unique_ptr<LightSystem> m_lightSystem;
            std::unique_ptr<CollisionSystem> m_collisionSystem;
            Transform * m_mainCameraTransform;
            config::Config m_configData;
            bool m_isRunning;
            float m_frameDeltaTimeFactor; //for slowing/pausing in debug editor

            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<utils::editor::EditorManager> m_editorManager;
            std::unique_ptr<nc::time::Timer> m_frameLogicTimer;
            #endif
            
            void FrameLogic(float dt);
            void FrameRender(float dt);
            void FrameCleanup();
            void FixedUpdate();
            void SendFrameUpdate(float dt) noexcept;
            void SendFixedUpdate() noexcept;
            void SendOnDestroy() noexcept;

            //returns true if entity is in Active or ToInitialize, false otherwise
            bool DoesEntityExist(const EntityHandle handle) const ; 

            //returns map containing entity, throws exception if not found
            auto& GetMapContainingEntity(const EntityHandle handle, bool checkAll = false) const noexcept(false);

            //returns ptr to entity regardless of map it's in (ToDestroy, etc.)
            Entity* GetEntityPtrFromAnyMap(const EntityHandle handle) const noexcept(false);
    };
} // end namespace nc::internal
