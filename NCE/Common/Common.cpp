#include "Common.hpp"

namespace NCE::Common
{
    Entity::Entity(int t_id, CreateEntityFunc t_createFunc, DestroyEntityFunc t_destroyFunc) : _id(t_id), CreateEntity(t_createFunc), DestroyEntity(t_destroyFunc)
    {

    }
    Entity::~Entity()
    {
        _components.clear();
    }

    int Entity::GetID() const
    {
        return _id;
    }

    void Entity::SendInitializeToComponents()
    {
        for(auto& current : _components)
        {
            current->OnInitialize();
        }
    }

    void Entity::SendFrameUpdateToComponents()
    {
        for(auto& current : _components)
        {
            current->OnFrameUpdate();
        }
    }

    void Entity::SendFixedUpdateToComponents()
    {
        for(auto& current : _components)
        {
            current->OnFixedUpdate();
        }
    }

    void Entity::SendDestroyToComponents()
    {
        for(auto& current : _components)
        {
            current->OnDestroy();
        }

        _components.clear();
    }

    void Entity::SendCollisionEnterToComponents(EntityWeakPtr t_other)
    {
        for(auto& comp : _components)
        {
            comp->OnCollisionEnter(t_other);
        }
    }

    void Entity::SendCollisionStayToComponents(EntityWeakPtr t_other)
    {
        for(auto& comp : _components)
        {
            comp->OnCollisionStay(t_other);
        }
    }

    // void Entity::SendCollisionExit(EntityWeakPtr t_other)
    // {
    //     for(auto& comp : _components)
    //     {
    //         comp->OnCollisionExit(t_other);
    //     }
    // }
    void Entity::SendCollisionExitToComponents()
    {
        for(auto& comp : _components)
        {
            comp->OnCollisionExit();
        }
    }

}