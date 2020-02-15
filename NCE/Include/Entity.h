#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <type_traits> //is_base_of

#include "Common.h"
#include "External.h"
#include "Component.h"
#include "Transform.h"

namespace nc
{
    class Entity
    {
        private:
            std::vector<std::shared_ptr<Component>> m_components;

        public:
            Entity(EntityHandle handle) noexcept : Handle(handle), TransformHandle(0) {}
            const EntityHandle Handle;
            ComponentHandle TransformHandle;
            
            void SendOnInitialize() noexcept;
            void SendFrameUpdate() noexcept;
            void SendFixedUpdate() noexcept;
            void SendOnDestroy() noexcept;
            void SendOnCollisionEnter(const EntityHandle other) noexcept;
            void SendOnCollisionStay() noexcept;
            void SendOnCollisionExit() noexcept;

            Transform* GetTransform() const noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool HasComponent() const noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool AddComponent() noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool RemoveComponent() noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            std::shared_ptr<T> GetComponent() const noexcept; 
    };

    //template definitions

    template<class T, class>
    bool Entity::HasComponent() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_components)
        {
            if (typeid(*item) == targetType) 
                return true;
        }
        return false;
    }

    template<class T, class>
    bool Entity::AddComponent() noexcept
    {
        if (HasComponent<T>()) return false;
        m_components.push_back(std::make_shared<T>(0, Handle));
        return true;
    }

    template<class T, class>
    bool Entity::RemoveComponent() noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(std::vector<Component>::size_type i = 0; i < m_components.size(); ++i)
        {
            if (typeid(*m_components.at(i)) == targetType)
            {
                m_components.at(i) = m_components.at(m_components.size() - 1);
                m_components.pop_back();
                return true;
            }
        }
        return false;        
    }

    template<class T, class>
    std::shared_ptr<T> Entity::GetComponent() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_components)
        {
            if (typeid(*item) == targetType)
                return std::dynamic_pointer_cast<T>(item);
        }

        return nullptr; //doesn't have component
    }
} //end namespace nc

#endif