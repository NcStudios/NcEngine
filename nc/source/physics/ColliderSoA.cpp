#include "ColliderSoA.h"
#include "Ecs.h"

namespace nc::physics
{
    ColliderSoA::ColliderSoA(size_t maxColliders)
        : m_handles(maxColliders), // braced init is ambiguous
          m_transforms{maxColliders},
          m_volumeProperties{maxColliders},
          m_types{maxColliders},
          m_gaps{},
          m_nextFree{0u}
    {
    }

    auto ColliderSoA::GetHandles() const noexcept -> const std::vector<EntityHandle::Handle_t>&
    {
        return m_handles;
    }

    auto ColliderSoA::GetTransforms() const noexcept -> const std::vector<const DirectX::XMMATRIX*>&
    {
        return m_transforms;
    }

    auto ColliderSoA::GetVolumeProperties() const noexcept -> const std::vector<VolumeProperties>&
    {
        return m_volumeProperties;
    }

    auto ColliderSoA::GetTypes() const noexcept -> const std::vector<ColliderType>&
    {
        return m_types;
    }

    void ColliderSoA::Add(EntityHandle handle, const ColliderInfo& info)
    {
        uint32_t pos = [this]()
        {
            if(m_gaps.empty())
                return m_nextFree++;
            
            auto out = m_gaps.back();
            m_gaps.pop_back();
            return out;
        }();

        if(m_nextFree >= m_handles.capacity())
            throw std::runtime_error("ColliderSoA::Add - exceeded capacity");

        m_handles[pos] = static_cast<EntityHandle::Handle_t>(handle);
        m_transforms[pos] = &GetComponent<Transform>(handle)->GetTransformationMatrix();
        m_volumeProperties[pos] = GetVolumePropertiesFromColliderInfo(info);
        m_types[pos] = info.type;
    }

    void ColliderSoA::Remove(EntityHandle handle)
    {
        // consideration - what if entity removes collider then adds again? does this work?
        auto pos = std::find(m_handles.cbegin(), m_handles.cend(), static_cast<EntityHandle::Handle_t>(handle));
        if(pos == m_handles.cend())
            throw std::runtime_error("ColliderSoA::Remove - handle doesn't exist");
        
        auto index = pos - m_handles.cbegin();
        m_gaps.push_back(index);
        std::sort(m_gaps.begin(), m_gaps.end());
    }

    void ColliderSoA::Clear()
    {
        m_gaps.resize(0u);
        m_nextFree = 0u;
    }

    void ColliderSoA::CalculateEstimates(std::vector<DynamicEstimate>* out)
    {
        uint32_t i = 0u;
        auto gapCurrent = m_gaps.cbegin();
        auto gapEnd = m_gaps.cend();

        while(i < m_nextFree)
        {
            if(gapCurrent != gapEnd && i == *gapCurrent)
            {
                ++gapCurrent; ++i; continue;
            }

            out->emplace_back(EstimateBoundingVolume(m_volumeProperties[i], m_transforms[i]), i);
            ++i;
        }
    }
} // namespace nc::physics