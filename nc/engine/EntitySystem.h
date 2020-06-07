#pragma once

#include "Entity.h"
#include "HandleManager.h"

#include <unordered_map>

namespace nc
{

class Transform;

namespace engine
{

class EntitySystem
{
    public:
        EntitySystem();

        EntityView CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, const std::string& tag); //creates new Entity and Transform and adds it to ToInitialize, returns handle to Entity
        bool DestroyEntity(EntityHandle handle);   //moves entity from current map to ToDestroy, returns true if successful
        Transform* GetTransformPtr(ComponentHandle handle); //returns ptr to Transform with given handle, returns nullptr if not found
        Entity* GetEntity(EntityHandle handle);    //returns ptr to entity in Active or ToInitialize maps, returns nullptr if not found
        Entity* GetEntity(const std::string& tag); //returns pointer to first active found entity with tag or nullptr if not found

        DoesEntityExist(EntityHandle handle);

    private:
        HandleManager<EntityHandle> m_handleManager;

        struct EntityMaps
        {
            /** @todo need entity graphs to support entity hierarchies */
            using EntityMap_t = std::unordered_map<EntityHandle, Entity>;
            EntityMap_t ToInitialize;
            EntityMap_t Active;
            EntityMap_t ToDestroy;
        } m_entityMaps;

        bool    DoesEntityExist(EntityHandle handle); //returns true if entity is in Active or ToInitialize, false otherwise
        auto&   GetMapContainingEntity(EntityHandle handle, bool checkAll = false) noexcept(false); //returns map containing entity, throws exception if not found
        Entity* GetEntityPtrFromAnyMap(EntityHandle handle) noexcept(false); //returns ptr to entity regardless of map it's in (ToDestroy, etc.)
};

} //end namespace engine
} //end namespace nc