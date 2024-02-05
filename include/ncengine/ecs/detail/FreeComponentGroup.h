#pragma once

#include "ncengine/ecs/Component.h"

#include "ncutility/NcError.h"

#include <concepts>
#include <memory>
#include <set>
#include <vector>

namespace nc::ecs::detail
{
class FreeComponentGroup final : public ComponentBase
{
    public:
        FreeComponentGroup(Entity entity)
            : ComponentBase{entity},
              m_components{},
              m_toAdd{},
              m_toRemove{}
        {
        }

        ~FreeComponentGroup() = default;
        FreeComponentGroup(FreeComponentGroup&&) = default;
        FreeComponentGroup& operator=(FreeComponentGroup&&) = default;
        FreeComponentGroup(const FreeComponentGroup&) = delete;
        FreeComponentGroup& operator=(const FreeComponentGroup&) = delete;

        template<std::derived_from<FreeComponent> T, class ... Args>
        auto Add(Args&& ... args) -> T&;

        template<std::derived_from<FreeComponent> T>
        auto Remove() -> bool;

        template<std::derived_from<FreeComponent> T>
        bool Contains() const noexcept;

        template<std::derived_from<FreeComponent> T>
        auto Get() const -> T&;

        template<class F>
        void ForEach(F&& func);

        auto GetComponents() const noexcept -> std::vector<FreeComponent*>;
        void CommitStagedComponents();

    private:
        std::vector<std::unique_ptr<FreeComponent>> m_components;
        std::vector<std::unique_ptr<FreeComponent>> m_toAdd;
        std::set<size_t> m_toRemove;
};

template<std::derived_from<FreeComponent> T, class ... Args>
auto FreeComponentGroup::Add(Args&& ... args) -> T&
{
    NC_ASSERT(!Contains<T>(), "Entity already has a component of this type");
    auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
    auto& out = *newComponent;
    m_toAdd.push_back(std::move(newComponent));
    return out;
}

template<std::derived_from<FreeComponent> T>
auto FreeComponentGroup::Remove() -> bool
{
    for(size_t i = 0; auto& component : m_components)
    {
        if(dynamic_cast<T*>(component.get()))
        {
            m_toRemove.insert(i);
            return true;
        }

        ++i;
    }

    return false;
}

template<std::derived_from<FreeComponent> T>
bool FreeComponentGroup::Contains() const noexcept
{
    for(auto& item : m_components)
    {
        auto* ptr = dynamic_cast<T*>(item.get());
        if(ptr)
            return true;
    }

    for(auto& item : m_toAdd)
    {
        auto* ptr = dynamic_cast<T*>(item.get());
        if(ptr)
            return true;
    }

    return false;
}

template<std::derived_from<FreeComponent> T>
auto FreeComponentGroup::Get() const -> T&
{
    for(auto& item : m_components)
    {
        auto* ptr = dynamic_cast<T*>(item.get());
        if(ptr)
            return *ptr;
    }

    for(auto& item : m_toAdd)
    {
        auto* ptr = dynamic_cast<T*>(item.get());
        if(ptr)
            return *ptr;
    }

    throw NcError("Component does not exist");
}

template<class F>
void FreeComponentGroup::ForEach(F&& func)
{
    for (auto& component : m_components)
    {
        func(*component);
    }
}
} // namespace nc::ecs::detail
