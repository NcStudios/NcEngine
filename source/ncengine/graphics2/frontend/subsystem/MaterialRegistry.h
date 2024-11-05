#pragma once

#include "graphics2/MaterialProperties.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/type/StableAddress.h"

#include <functional>
#include <vector>

namespace nc::graphics
{
class MaterialRegistry : public StableAddress
{
    public:
        explicit MaterialRegistry(uint32_t maxInstances);

        auto CreateInstance(const MaterialDesc& desc = MaterialDesc{}) -> MaterialInstanceHandle;
        void DestroyInstance(MaterialInstanceHandle index) noexcept;
        auto GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&;
        void SetInstanceDesc(MaterialInstanceHandle index, const MaterialDesc& desc);
        auto GetInstanceProperties(MaterialInstanceHandle index) const -> const MaterialProperties&;
        auto HasPendingChanges() const -> bool;
        void CommitPendingChanges(std::function<void(const MaterialPropertyUpdateInfo&)> notifyUpdate);

    private:
        std::vector<MaterialProperties> m_properties;
        std::vector<MaterialDesc> m_descriptions;
        std::vector<MaterialInstanceHandle> m_dirty;
        std::vector<MaterialInstanceHandle> m_freeList;
        MaterialInstanceHandle m_nextIndex = 0;
        MaterialInstanceHandle m_maxIndex;

        auto CollectDirtyRanges() -> std::vector<UpdateRange>;
};
} // namespace nc::graphics
