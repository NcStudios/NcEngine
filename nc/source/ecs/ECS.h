#pragma once
#include "NcCommon.h"
#include "HandleManager.h"

#include <unordered_map>

namespace nc
{
    class Renderer;
    class PointLight;
}

namespace nc::ecs
{
    class LightSystem;
    class RenderingSystem;
    class TransformSystem;
    
    class ECS
    {
        using EntityMap = std::unordered_map<EntityHandle, Entity>;

        public:
            ECS(LightSystem* light, RenderingSystem* rend, TransformSystem* trans);
            ~ECS();

            static RenderingSystem* GetRenderingSystem();

            static EntityHandle CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag);
            static bool DestroyEntity(EntityHandle handle);
            static Entity* GetEntity(EntityHandle handle);
            static Entity* GetEntity(std::string tag);
            static Transform* GetTransformFromComponentHandle(ComponentHandle transformHandle);
            static Transform* GetTransformFromEntityHandle(EntityHandle entityHandle);

            template<class T> static bool HasComponent(EntityHandle handle);
            template<class T> static bool RemoveComponent(EntityHandle handle);
            template<class T> static T* GetComponent(EntityHandle handle);
            static Renderer* AddRenderer(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material);
            static PointLight* AddPointLight(EntityHandle handle);

            bool DoesEntityExist(const EntityHandle handle) noexcept;
            Entity * GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false);
            EntityMap & GetMapContainingEntity(const EntityHandle handle, bool checkAll = false) noexcept(false);
            EntityMap & GetActiveEntities() noexcept;
            EntityMap & GetToDestroyEntities() noexcept;

            void ClearSystems();
            void RemoveTransform(ComponentHandle handle);

            HandleManager<EntityHandle> handleManager;
            
        private:
            static ECS* m_instance;
            EntityMap m_active;
            EntityMap m_toDestroy;
            LightSystem* m_lightSystem;
            RenderingSystem* m_renderingSystem;
            TransformSystem* m_transformSystem;
    };
}