#pragma once

#include "component/Component.h"
#include "debug/Utils.h"

#include <concepts>
#include <memory>
#include <set>
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
            std::set<size_t> m_toRemove;
    };

    template<std::derived_from<AutoComponent> T, class ... Args>
    T* AutoComponentGroup::Add(Args&& ... args)
    {
        IF_THROW(Contains<T>(), "Entity already has component of this type");
        auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
        auto* out = newComponent.get();
        m_toAdd.push_back(std::move(newComponent));
        return out;
    }

    template<std::derived_from<AutoComponent> T>
    void AutoComponentGroup::Remove()
    {
        for(size_t i = 0; auto& component : m_components)
        {
            if(dynamic_cast<T*>(component.get()))
            {
                m_toRemove.insert(i);
                return;
            }

            ++i;
        }

        throw NcError("Component does not exist");
    }

    template<std::derived_from<AutoComponent> T>
    bool AutoComponentGroup::Contains() const noexcept
    {
        return Get<T>() != nullptr;
    }

    template<std::derived_from<AutoComponent> T>
    T* AutoComponentGroup::Get() const noexcept
    {
        for(auto& item : m_components)
        {
            auto* ptr = dynamic_cast<T*>(item.get());
            if(ptr)
                return ptr;
        }

        return nullptr;
    }
}