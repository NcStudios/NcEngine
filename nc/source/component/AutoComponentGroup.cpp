#include "component/AutoComponentGroup.h"

namespace nc
{
    AutoComponentGroup::AutoComponentGroup(Entity)
        : m_components{},
          m_toAdd{},
          m_toRemove{}
    {}

    auto AutoComponentGroup::GetAutoComponents() const noexcept -> std::vector<AutoComponent*>
    {
        std::vector<AutoComponent*> out(m_components.size());
        std::transform(m_components.cbegin(), m_components.cend(), out.begin(), [](const auto& ptr)
        {
            return ptr.get();
        });
        return out;
    }

    void AutoComponentGroup::SendFrameUpdate(float dt)
    {
        for(auto& comp : m_components)
        {
            comp->FrameUpdate(dt);
        }
    }

    void AutoComponentGroup::SendFixedUpdate()
    {
        for(auto& comp : m_components)
        {
            comp->FixedUpdate();
        }
    }

    void AutoComponentGroup::SendOnDestroy()
    {
        for(auto& comp : m_components)
        {
            comp->OnDestroy();
        }
    }

    void AutoComponentGroup::SendOnCollisionEnter(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionEnter(hit);
        }
    }

    void AutoComponentGroup::SendOnCollisionStay(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionStay(hit);
        }
    }

    void AutoComponentGroup::SendOnCollisionExit(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnCollisionExit(hit);
        }
    }

    void AutoComponentGroup::SendOnTriggerEnter(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnTriggerEnter(hit);
        }
    }

    void AutoComponentGroup::SendOnTriggerStay(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnTriggerStay(hit);
        }
    }

    void AutoComponentGroup::SendOnTriggerExit(Entity hit)
    {
        for(auto& comp : m_components)
        {
            comp->OnTriggerExit(hit);
        }
    }

    void AutoComponentGroup::CommitStagedComponents()
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
}