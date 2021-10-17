#pragma once

#include "component/Component.h"
#include "debug/Utils.h"

#include <concepts>
#include <memory>
#include <vector>

namespace nc
{
    class AutoComponentGroup final
    {
        public:
            AutoComponentGroup(Entity entity);
            AutoComponentGroup(AutoComponentGroup&&) = default;
            AutoComponentGroup& operator=(AutoComponentGroup&&) = default;
            ~AutoComponentGroup() = default;

            AutoComponentGroup(const AutoComponentGroup&) = delete;
            AutoComponentGroup& operator=(const AutoComponentGroup&) = delete;

            template<std::derived_from<AutoComponent> T, class ... Args>
            T* Add(Args&& ... args);

            template<std::derived_from<AutoComponent> T>
            void Remove();

            template<std::derived_from<AutoComponent> T>
            bool Contains() const noexcept;

            template<std::derived_from<AutoComponent> T>
            T* Get() const noexcept; 

            auto GetAutoComponents() const noexcept -> std::vector<AutoComponent*>;
            void CommitStagedComponents();

            void SendFrameUpdate(float dt);
            void SendFixedUpdate();
            void SendOnDestroy();
            void SendOnCollisionEnter(Entity hit);
            void SendOnCollisionStay(Entity hit);
            void SendOnCollisionExit(Entity hit);
            void SendOnTriggerEnter(Entity hit);
            void SendOnTriggerStay(Entity hit);
            void SendOnTriggerExit(Entity hit);

        private:
            std::vector<std::unique_ptr<AutoComponent>> m_components;
            std::vector<std::unique_ptr<AutoComponent>> m_toAdd;
    };

    template<std::derived_from<AutoComponent> T, class ... Args>
    T* AutoComponentGroup::Add(Args&& ... args)
    {
        IF_THROW(Contains<T>(), "Entity already has component of this type");
        m_toAdd.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return dynamic_cast<T*>(m_toAdd.back().get());
    }

    template<std::derived_from<AutoComponent> T>
    void AutoComponentGroup::Remove()
    {
        const std::type_info &targetType(typeid(T));
        for(auto& comp : m_components)
        {
            if(typeid(*comp) == targetType)
            {
                comp = nullptr;
                return;
            }
        }

        throw NcError("Component does not exist");
    }

    template<std::derived_from<AutoComponent> T>
    bool AutoComponentGroup::Contains() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_components)
        {
            if (typeid(*item) == targetType) 
                return true;
        }
        return false;
    }

    template<std::derived_from<AutoComponent> T>
    T* AutoComponentGroup::Get() const noexcept
    {
        const std::type_info &targetType(typeid(T));
        for(auto& item : m_components)
        {
            if (typeid(*item) == targetType)
                return dynamic_cast<T*>(item.get());
        }

        return nullptr;
    }
}