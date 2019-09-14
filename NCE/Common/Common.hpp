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
            EntityWeakPtr _parentEntity;
            
        public:
            uint32_t TypeId;

            Component(EntityWeakPtr t_parent) { _parentEntity = t_parent; }
            //~Component() { _parentEntity.lock() = nullptr; }

            EntityWeakPtr GetEntity() 
            { 
                //std::shared_ptr<NCE::Common::Entity> _p = _parentEntity.lock();

                    return _parentEntity;

            }
            
            virtual void OnInitialize(){}
            virtual void OnDestroy(){}  
            virtual void OnEnable(){}
            virtual void OnDisable(){}
            virtual void OnFrameUpdate(){}
            virtual void OnFixedUpdate(){}
            virtual void OnCollisionEnter(EntityWeakPtr t_other){}
            virtual void OnCollisionStay(EntityWeakPtr t_other){}
            //virtual void OnCollisionExit(EntityWeakPtr t_other){}   
            virtual void OnCollisionExit(){}
    };


    class Entity
    {
        private:
            ComponentSharedPtrVector _components;
            int _id;

        public:
            Entity(int t_id, CreateEntityFunc t_createFunc, DestroyEntityFunc t_destroyFunc);
            ~Entity();

            int GetID() const;

            template<class T>
            void AddComponent(EntityWeakPtr t_parent);

            template<class T>
            std::weak_ptr<T> GetComponent(uint32_t t_componentId);

            void SendInitializeToComponents();
            void SendFrameUpdateToComponents();
            void SendFixedUpdateToComponents();
            void SendDestroyToComponents();

            void SendCollisionEnterToComponents(EntityWeakPtr t_other);
            void SendCollisionStayToComponents(EntityWeakPtr t_other);
            //void SendCollisionExit(EntityWeakPtr t_other);
            void SendCollisionExitToComponents();

            CreateEntityFunc CreateEntity;
            DestroyEntityFunc DestroyEntity;
    };
}

#include "Common_template_impl.hpp"


#endif