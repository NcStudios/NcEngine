#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Common.h"
#include "views/EntityView.h"
#include "component/EngineComponentGroup.h"

namespace nc
{
    class Component; 
    class Transform;

    class Entity
    {
        public:
            Entity(EntityHandle handle, const std::string& tag = "") noexcept;
            Entity(const Entity& other) = default;
            Entity(Entity&& other) = default;
            ~Entity() = default;

            const EntityHandle Handle;
            const std::string Tag;
            EngineComponentHandleGroup Handles;
            
            //could make engine a friend and make these private
            void SendOnInitialize() noexcept;
            void SendFrameUpdate(float dt) noexcept;
            void SendFixedUpdate() noexcept;
            void SendOnDestroy() noexcept;
            void SendOnCollisionEnter(const EntityHandle other) noexcept;
            void SendOnCollisionStay() noexcept;
            void SendOnCollisionExit() noexcept;

            Transform* GetTransform() const noexcept;

            template<class T, 
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool HasUserComponent() const noexcept;

            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type,
                     class ... Args>
            T * AddUserComponent(Args&& ... args) noexcept;

            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            bool RemoveUserComponent() noexcept;

            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            T * GetUserComponent() const noexcept; 

            std::vector<std::shared_ptr<Component>> GetUserComponents() const noexcept;
        
        private:
            std::vector<std::shared_ptr<Component>> m_userComponents;
    };

    //template definitions
    template<class T, class>
    bool Entity::HasUserComponent() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_userComponents)
        {
            if (typeid(*item) == targetType) 
                return true;
        }
        return false;
    }

    template<class T, class, class ... Args>
    T * Entity::AddUserComponent(Args&& ... args) noexcept
    {
        if (HasUserComponent<T>()) return nullptr;
        auto newComponent = std::make_shared<T>(std::forward<Args>(args)...);
        std::dynamic_pointer_cast<Component>(newComponent)->Initialize(0, EntityView(Handle, Handles.transform));
        m_userComponents.push_back(newComponent);
        return newComponent.get();
    }

    template<class T, class>
    bool Entity::RemoveUserComponent() noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(std::vector<Component>::size_type i = 0; i < m_userComponents.size(); ++i)
        {
            if (typeid(*m_userComponents.at(i)) == targetType)
            {
                m_userComponents.at(i) = m_userComponents.at(m_userComponents.size() - 1);
                m_userComponents.pop_back();
                return true;
            }
        }
        return false;        
    }

    template<class T, class>
    T * Entity::GetUserComponent() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_userComponents)
        {
            if (typeid(*item) == targetType)
                return dynamic_cast<T*>(item.get());
        }

        return nullptr; //doesn't have component
    }

} //end namespace nc