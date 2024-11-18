#include "MaterialRegistry.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <ranges>

namespace
{
auto ToMaterialData(const nc::MaterialProperties& properties) -> nc::graphics::MaterialData
{
    return nc::graphics::MaterialData{
        .gradientStart = properties.gradientStart,
        .diffuseTexIndex = properties.diffuseTexture.index,
        .gradientEnd = properties.gradientEnd,
        .normalTexIndex = properties.normalTexture.index,
        .outlineColor = properties.outlineColor,
        .outlineWidth = properties.outlineWidth
    };
}
} // anonymous namespace

namespace nc::graphics
{
MaterialRegistry::MaterialRegistry(uint32_t maxInstances)
    : m_maxIndex{maxInstances}
{
    MaterialInstance::s_registry = this;
}

auto MaterialRegistry::CreateInstance(const MaterialDesc& desc) -> MaterialInstanceHandle
{
    if (m_freeList.empty())
    {
        NC_ASSERT(m_nextIndex < m_maxIndex, fmt::format("Max material instances exceeded: current {}, max {}", m_nextIndex, m_maxIndex));
        m_data.push_back(ToMaterialData(desc.properties));
        m_descriptions.push_back(desc);
        m_dirty.push_back(m_nextIndex);
        return m_nextIndex++;
    }

    const auto index = m_freeList.back();
    m_freeList.pop_back();
    m_data[index] = ToMaterialData(desc.properties);
    m_descriptions[index] = desc;
    m_dirty.push_back(index);
    return index;
}

void MaterialRegistry::DestroyInstance(MaterialInstanceHandle index)
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    m_descriptions[index] = MaterialDesc{};
    m_freeList.push_back(index);
}

auto MaterialRegistry::GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    return m_descriptions[index];
}

void MaterialRegistry::SetInstanceName(MaterialInstanceHandle index, std::string_view name)
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    m_descriptions[index].name = std::string{name};
}

void MaterialRegistry::SetInstanceProperties(MaterialInstanceHandle index, const MaterialProperties& properties)
{
    NC_ASSERT(index < m_data.size(), "Invalid MaterialInstanceHandle");
    m_data[index] = ToMaterialData(properties);
    m_descriptions[index].properties = properties;
    m_dirty.push_back(index);
}

auto MaterialRegistry::GetInstanceData(MaterialInstanceHandle index) const -> const MaterialData&
{
    NC_ASSERT(index < m_data.size(), "Invalid MaterialInstanceHandle");
    return m_data[index];
}

auto MaterialRegistry::HasPendingChanges() const -> bool
{
    return !m_dirty.empty();
}

auto MaterialRegistry::BuildState() -> BufferUpdateInfo<MaterialData>
{
    return HasPendingChanges()
        ? BufferUpdateInfo<MaterialData>{m_data, CollectDirtyRanges()}
        : BufferUpdateInfo<MaterialData>{};
}

void MaterialRegistry::CommitPendingChanges(std::function<void(const BufferUpdateInfo<MaterialData>&)> notifyUpdate)
{
    notifyUpdate(BufferUpdateInfo<MaterialData>{
        .instances = m_data,
        .dirtyRanges = CollectDirtyRanges()
    });
}

auto MaterialRegistry::CollectDirtyRanges() -> std::vector<BufferSlice>
{
    auto out = std::vector<BufferSlice>{};
    if (m_dirty.empty())
    {
        return out;
    }

    std::ranges::sort(m_dirty);
    auto removed = std::ranges::unique(m_dirty);
    m_dirty.erase(removed.begin(), removed.end());
    auto start = m_dirty[0];
    auto end = start + 1;
    for (const auto nextEnd : std::views::drop(m_dirty, 1))
    {
        if (nextEnd == end)
        {
            end += 1;
        }
        else
        {
            out.emplace_back(start, end - start);
            start = nextEnd;
            end = start + 1;
        }
    }

    out.emplace_back(start, end - start);
    m_dirty.clear();
    return out;
}
} // namespace nc::graphics
