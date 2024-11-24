#pragma once

#include "HostStructuredBuffer.h"
#include "graphics2/ShaderTypes.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/type/StableAddress.h"

#include <vector>

namespace nc::graphics
{
class MaterialRegistry : public StableAddress
{
    public:
        explicit MaterialRegistry(uint32_t maxInstances);

        // API-Facing Functions
        auto CreateInstance(const MaterialDesc& desc = MaterialDesc{}) -> MaterialInstanceHandle;
        void DestroyInstance(MaterialInstanceHandle index);
        auto GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&;
        void SetInstanceProperties(MaterialInstanceHandle index, const MaterialProperties& properties);
        auto GetInstanceData(MaterialInstanceHandle index) const -> const MaterialData&;
        void SetInstanceName(MaterialInstanceHandle index, std::string_view name);

        // Graphics Frontend Functions
        auto BuildState() -> BufferUpdateInfo<MaterialData>;

    private:
        HostStructuredBuffer<MaterialData> m_buffer;
        std::vector<MaterialDesc> m_descriptions;
};
} // namespace nc::graphics
