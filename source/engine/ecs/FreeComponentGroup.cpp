#include "ncengine/ecs/detail/FreeComponentGroup.h"

#include <algorithm>

namespace nc::ecs::detail
{
FreeComponentGroup::FreeComponentGroup(Entity entity)
    : ComponentBase{entity},
        m_components{},
        m_toAdd{},
        m_toRemove{}
{}

auto FreeComponentGroup::GetComponents() const noexcept -> std::vector<FreeComponent*>
{
    std::vector<FreeComponent*> out(m_components.size());
    std::transform(m_components.cbegin(), m_components.cend(), out.begin(), [](const auto& ptr)
    {
        return ptr.get();
    });
    return out;
}

void FreeComponentGroup::CommitStagedComponents()
{
    for(auto i : m_toRemove)
    {
        m_components.at(i) = std::move(m_components.back());
        m_components.pop_back();
    }

    m_toRemove.clear();

    for(auto& toAdd : m_toAdd)
        m_components.push_back(std::move(toAdd));

    m_toAdd.clear();
}
} // namespace nc::ecs::detail
