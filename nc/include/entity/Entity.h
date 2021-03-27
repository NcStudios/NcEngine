#pragma once

#include "component/Component.h"
#include "physics/LayerMask.h"

#include <concepts>
#include <memory>
#include <string>
#include <vector>

namespace nc
{
    class Transform;

    class Entity
    {
        public:
            const EntityHandle Handle;
            const std::string Tag;
            const physics::Layer Layer;
            const bool IsStatic;

            Entity(EntityHandle handle, std::string tag, physics::Layer layer, bool isStatic) noexcept;
            Entity(const Entity& other) = delete;
            Entity& operator=(const Entity&) = delete;
            Entity(Entity&& other) = default;
            Entity& operator=(Entity&& other) = default;
            ~Entity() = default;

            void SendFrameUpdate(float dt) noexcept;
            void SendFixedUpdate() noexcept;
            void SendOnDestroy() noexcept;
            void SendOnCollisionEnter(Entity* other) noexcept;
            void SendOnCollisionStay(Entity* other) noexcept;
            void SendOnCollisionExit(Entity* other) noexcept;

            template<std::derived_from<Component> T, class ... Args>
            T * AddUserComponent(Args&& ... args) noexcept;

            template<std::derived_from<Component> T>
            T * GetUserComponent() const noexcept; 

            template<std::derived_from<Component> T>
            bool HasUserComponent() const noexcept;

            template<std::derived_from<Component> T>
            bool RemoveUserComponent() noexcept;

            std::vector<Component*> GetUserComponents() const noexcept;
        
        private:
            std::vector<std::unique_ptr<Component>> m_userComponents;
    };

    template<std::derived_from<Component> T, class ... Args>
    T * Entity::AddUserComponent(Args&& ... args) noexcept
    {
        if (HasUserComponent<T>())
        {
            return nullptr;
        }
        m_userComponents.emplace_back(std::make_unique<T>(Handle, std::forward<Args>(args)...));
        return dynamic_cast<T*>(m_userComponents.back().get());
    }

    template<std::derived_from<Component> T>
    T * Entity::GetUserComponent() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_userComponents)
        {
            if (typeid(*item) == targetType)
                return dynamic_cast<T*>(item.get());
        }

        return nullptr;
    }
    
    template<std::derived_from<Component> T>
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

    template<std::derived_from<Component> T>
    bool Entity::RemoveUserComponent() noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(std::vector<Component>::size_type i = 0; i < m_userComponents.size(); ++i)
        {
            if (typeid(*m_userComponents.at(i)) == targetType)
            {
                m_userComponents.at(i) = std::move(m_userComponents.at(m_userComponents.size() - 1));
                m_userComponents.pop_back();
                return true;
            }
        }
        return false;
    }
} //end namespace nc