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
        void SetInstanceProperties(MaterialInstanceHandle index, const MaterialProperties& properties);
        auto GetInstanceData(MaterialInstanceHandle index) const -> const MaterialData&;
        void SetInstanceName(MaterialInstanceHandle index, std::string_view name);
        auto HasPendingChanges() const -> bool;
        void CommitPendingChanges(std::function<void(const BufferUpdateInfo<MaterialData>&)> notifyUpdate);
        auto BuildState() -> BufferUpdateInfo<MaterialData>;

    private:
        std::vector<MaterialData> m_data;
        std::vector<MaterialDesc> m_descriptions;
        std::vector<uint32_t> m_dirty;
        std::vector<uint32_t> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;

        auto CollectDirtyRanges() -> std::vector<BufferSlice>;
};
} // namespace nc::graphics
