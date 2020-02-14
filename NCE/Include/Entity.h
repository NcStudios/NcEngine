#ifndef ENTITY
#define ENTITY

#include <vector>
#include <type_traits> //is_base_of

#include "Common.h"
//#include "NCE.h"

#include "External.h"
#include "Component.h"
#include "Transform.h"

class Entity
{
    private:
        std::vector<std::shared_ptr<Component>> m_components;

    public:
        Entity(EntityHandle handle) : Handle(handle), TransformHandle(0) {}
        const EntityHandle Handle;
        ComponentHandle TransformHandle;
        
        void SendOnInitialize();
        void SendFrameUpdate();
        void SendFixedUpdate();
        void SendOnDestroy();
        void SendOnCollisionEnter(const EntityHandle other);
        void SendOnCollisionStay();
        void SendOnCollisionExit();


        template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
        bool HasComponent()
        {
            for(auto& item : m_components)
            {
                if (typeid(*item) == typeid(T)) 
                    return true;
            }
            return false;
        }

        template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
        bool AddComponent()
        {
            if (HasComponent<T>()) return false;
            m_components.push_back(std::make_shared<T>(0, Handle));
            return true;
        }

        template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
        bool RemoveComponent(ComponentType type)
        {
            for(std::vector<Component>::size_type i = 0; i < m_components.size(); ++i)
            {
                if (typeid(*m_components.at(i)) == typeid(T))
                {
                    m_components.at(i) = m_components.at(m_components.size() - 1);
                    m_components.pop_back();
                    return true;
                }
            }
            return false;        
        }

        Transform* GetTransform();

        template<class T>
        std::shared_ptr<T> GetComponent()
        {
            if (!HasComponent<T>())
                return nullptr;

            for(auto& item : m_components)
            {
                if (typeid(T) == typeid(*item))
                    return std::dynamic_pointer_cast<T>(item);
                    //cast<shared_ptr<T> instead of T?
                    //return weak_ptr instead?
            }

            std::cout << "Entity-GetPointerTo -- this should never be reached, correct?\n";
            return nullptr;
        }
};

#endif