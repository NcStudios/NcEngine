#include "Common.hpp"

namespace NCE::Common
{
    Entity::Entity(int id_, CreateEntityFunc createFunc_, DestroyEntityFunc destroyFunc_) : m_id(id_), CreateEntity(createFunc_), DestroyEntity(destroyFunc_)
    {

    }
    Entity::~Entity()
    {
        m_components.clear();
    }

    int Entity::GetID() const
    {
        return m_id;
    }

    void Entity::SendInitializeToComponents()
    {
        for(auto& current : m_components)
        {
            current->OnInitialize();
        }
    }

    void Entity::SendFrameUpdateToComponents()
    {
        for(auto& current : m_components)
        {
            current->OnFrameUpdate();
        }
    }

    void Entity::SendFixedUpdateToComponents()
    {
        for(auto& current : m_components)
        {
            current->OnFixedUpdate();
        }
    }

    void Entity::SendDestroyToComponents()
    {
        for(auto& current : m_components)
        {
            current->OnDestroy();
        }

        m_components.clear();
    }

    void Entity::SendCollisionEnterToComponents(EntityWeakPtr other_)
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionEnter(other_);
        }
    }

    void Entity::SendCollisionStayToComponents(EntityWeakPtr other_)
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionStay(other_);
        }
    }

    // void Entity::SendCollisionExit(EntityWeakPtr other_)
    // {
    //     for(auto& comp : _components)
    //     {
    //         comp->OnCollisionExit(other_);
    //     }
    // }
    void Entity::SendCollisionExitToComponents()
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionExit();
        }
    }

}