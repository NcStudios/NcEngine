#pragma once

#include "graphics2/ShaderTypes.h"
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
        void DestroyInstance(MaterialInstanceHandle index);
        auto GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&;
        void SetInstanceDesc(MaterialInstanceHandle index, const MaterialDesc& desc);
        auto GetInstanceData(MaterialInstanceHandle index) const -> const MaterialData&;
        void SetInstanceName(MaterialInstanceHandle index, std::string_view name);
        auto HasPendingChanges() const -> bool;
        void CommitPendingChanges(std::function<void(const MaterialDataUpdateInfo&)> notifyUpdate);

    private:
        std::vector<MaterialData> m_data;
        std::vector<MaterialDesc> m_descriptions;
        std::vector<MaterialInstanceHandle> m_dirty;
        std::vector<MaterialInstanceHandle> m_freeList;
        MaterialInstanceHandle m_nextIndex = 0;
        MaterialInstanceHandle m_maxIndex;

        auto CollectDirtyRanges() -> std::vector<BufferSlice>;
};
} // namespace nc::graphics
