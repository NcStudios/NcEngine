#include "ColliderSoA.h"
#include "Ecs.h"

namespace nc::physics
{
    ColliderSoA::ColliderSoA(size_t maxColliders)
        : handles(maxColliders), // braced init is ambiguous
          transforms{maxColliders},
          volumeProperties{maxColliders},
          types{maxColliders},
          gaps{},
          nextFree{0u}
    {
    }

    auto ColliderSoA::GetHandles() const noexcept -> const std::vector<EntityHandle::Handle_t>&
    {
        return handles;
    }

    auto ColliderSoA::GetTransforms() const noexcept -> const std::vector<const DirectX::XMMATRIX*>&
    {
        return transforms;
    }

    auto ColliderSoA::GetVolumeProperties() const noexcept -> const std::vector<VolumeProperties>&
    {
        return volumeProperties;
    }

    auto ColliderSoA::GetTypes() const noexcept -> const std::vector<ColliderType>&
    {
        return types;
    }

    void ColliderSoA::Add(EntityHandle handle, const ColliderInfo& info)
    {
        // store maxColliders?
        // also how to balance max colliders acros SoA and tree
        if(nextFree >= handles.capacity())
            throw std::runtime_error("ColliderSoA::Add - exceeded capacity");


        uint32_t pos = [this]()
        {
            if(gaps.empty())
                return nextFree++;
            
            auto out = gaps.back();
            gaps.pop_back();
            return out;
        }();

        handles[pos] = static_cast<EntityHandle::Handle_t>(handle);
        transforms[pos] = &GetComponent<Transform>(handle)->GetTransformationMatrix();
        volumeProperties[pos] = GetVolumePropertiesFromColliderInfo(info);
        types[pos] = info.type;
    }

    void ColliderSoA::Remove(EntityHandle handle)
    {
        // consideration - what if entity removes collider then adds again? does this work?
        auto pos = std::find(handles.cbegin(), handles.cend(), static_cast<EntityHandle::Handle_t>(handle));
        if(pos == handles.cend())
            throw std::runtime_error("ColliderSoA::Remove - handle doesn't exist");
        
        auto index = pos - handles.cbegin();
        gaps.push_back(index);
        std::sort(gaps.begin(), gaps.end());
    }

    void ColliderSoA::Clear()
    {
        gaps.resize(0u);
        nextFree = 0u;
    }

    void ColliderSoA::CalculateEstimates(std::vector<DynamicEstimate>* out)
    {
        uint32_t i = 0u;
        auto gapCurrent = gaps.cbegin();
        auto gapEnd = gaps.cend();

        while(i < nextFree)
        {
            if(gapCurrent != gapEnd && i == *gapCurrent)
            {
                ++gapCurrent; ++i; continue;
            }

            out->emplace_back(EstimateBoundingVolume(volumeProperties[i], transforms[i]), i);
            ++i;
        }
    }
} // namespace nc::physics