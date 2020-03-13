#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Common.h"
#include "EntityView.h"

namespace nc
{
    class Component; 
    class Transform;

    class Entity
    {
        public:
            Entity(EntityHandle handle, const std::string& tag = "") noexcept : Handle(handle), TransformHandle(0), Tag(tag) {}
            const EntityHandle Handle;
            ComponentHandle TransformHandle;
            ComponentHandle RendererHandle;
            const std::string Tag;
            
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
            std::shared_ptr<T> AddComponent() noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool RemoveComponent() noexcept;

            template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            std::shared_ptr<T> GetComponent() const noexcept; 
        
        private:
            std::vector<std::shared_ptr<Component>> m_components;
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
    std::shared_ptr<T> Entity::AddComponent() noexcept
    {
        if (HasComponent<T>()) return nullptr;
        EntityView view(Handle, TransformHandle);
        auto newComponent = std::make_shared<T>(0, view);
        m_components.push_back(newComponent);
        return newComponent;
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