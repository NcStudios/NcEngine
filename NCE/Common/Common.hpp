#ifndef NCE_COMMON
#define NCE_COMMON

#include "../Components/_ComponentID.hpp"

#include <memory>
#include <vector>
#include <typeinfo>

#include <iostream>


namespace NCE::Common
{
    class Component;
    class Entity;


    typedef std::shared_ptr<NCE::Common::Entity> EntitySharedPtr;
    typedef std::shared_ptr<NCE::Common::Component>  ComponentSharedPtr;
    typedef std::vector<EntitySharedPtr> EntitySharedPtrVector;
    typedef std::vector<ComponentSharedPtr> ComponentSharedPtrVector;

    typedef std::weak_ptr<NCE::Common::Entity> EntityWeakPtr;
    typedef std::shared_ptr<NCE::Common::Entity> ComponentWeakPtr;
    typedef std::vector<EntityWeakPtr> EntityWeakPtrVector;
    typedef std::vector<ComponentWeakPtr> ComponentWeakPtrVector;

    typedef EntityWeakPtr(*CreateEntityFunc)();
    typedef void(*DestroyEntityFunc)(int);

    class Component
    {            
        private:
            //EntityPtr _parentEntity;
            EntityWeakPtr m_parentEntity;
            
        public:
            uint32_t TypeId;

            Component(EntityWeakPtr parent_) { m_parentEntity = parent_; }
            //~Component() { _parentEntity.lock() = nullptr; }

            EntityWeakPtr GetEntity() 
            { 
                //std::shared_ptr<NCE::Common::Entity> _p = _parentEntity.lock();

                    return m_parentEntity;

            }
            
            virtual void OnInitialize(){}
            virtual void OnDestroy(){}  
            virtual void OnEnable(){}
            virtual void OnDisable(){}
            virtual void OnFrameUpdate(){}
            virtual void OnFixedUpdate(){}
            virtual void OnCollisionEnter(EntityWeakPtr other_){}
            virtual void OnCollisionStay(EntityWeakPtr other_){}
            //virtual void OnCollisionExit(EntityWeakPtr t_other){}   
            virtual void OnCollisionExit(){}
    };


    class Entity
    {
        private:
            ComponentSharedPtrVector m_components;
            int m_id;

        public:
            Entity(int id_, CreateEntityFunc createFunc_, DestroyEntityFunc destroyFunc_);
            ~Entity();

            int GetID() const;

            template<class T>
            void AddComponent(EntityWeakPtr parent_);

            template<class T>
            std::weak_ptr<T> GetComponent(uint32_t componentId_);

            void SendInitializeToComponents();
            void SendFrameUpdateToComponents();
            void SendFixedUpdateToComponents();
            void SendDestroyToComponents();

            void SendCollisionEnterToComponents(EntityWeakPtr other_);
            void SendCollisionStayToComponents(EntityWeakPtr other_);
            //void SendCollisionExit(EntityWeakPtr t_other);
            void SendCollisionExitToComponents();

            CreateEntityFunc CreateEntity;
            DestroyEntityFunc DestroyEntity;
    };
}

#include "Common_template_impl.hpp"


#endif