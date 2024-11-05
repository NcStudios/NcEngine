#include "MaterialRegistry.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <ranges>

namespace
{
auto ToMaterialData(const nc::MaterialDesc& desc) -> nc::graphics::MaterialData
{
    return nc::graphics::MaterialData{
        desc.diffuseTexture.index,
        desc.normalTexture.index,
        desc.gradientStart,
        desc.gradientEnd,
        desc.outlineColor,
        desc.outlineWidth
    };
}
} // anonymous namespace

namespace nc::graphics
{
MaterialRegistry::MaterialRegistry(uint32_t maxInstances)
    : m_maxIndex{static_cast<MaterialInstanceHandle>(maxInstances)}
{
    MaterialInstance::s_registry = this;
}

auto MaterialRegistry::CreateInstance(const MaterialDesc& desc) -> MaterialInstanceHandle
{
    if (m_freeList.empty())
    {
        NC_ASSERT(m_nextIndex < m_maxIndex, "Max material instances exceeded");
        m_data.push_back(ToMaterialData(desc));
        m_descriptions.push_back(desc);
        m_dirty.push_back(m_nextIndex);
        return m_nextIndex++;
    }

    const auto index = m_freeList.back();
    m_freeList.pop_back();
    m_data[index] = ToMaterialData(desc);
    m_descriptions[index] = desc;
    m_dirty.push_back(index);
    return index;
}

void MaterialRegistry::DestroyInstance(MaterialInstanceHandle index) noexcept
{
    // Ignore out-of-bounds so ~MaterialInstance can be noexcept
    if (index < m_descriptions.size())
    {
        m_descriptions[index] = MaterialDesc{};
        m_freeList.push_back(index);
    }
}

auto MaterialRegistry::GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    return m_descriptions[index];
}

void MaterialRegistry::SetInstanceDesc(MaterialInstanceHandle index, const MaterialDesc& desc)
{
    NC_ASSERT(index < m_data.size(), "Invalid MaterialInstanceHandle");
    m_data[index] = ToMaterialData(desc);
    m_descriptions[index] = desc;
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

void MaterialRegistry::CommitPendingChanges(std::function<void(const MaterialDataUpdateInfo&)> notifyUpdate)
{
    notifyUpdate(MaterialDataUpdateInfo{
        .instances = m_data,
        .dirtyRanges = CollectDirtyRanges()
    });
}

auto MaterialRegistry::CollectDirtyRanges() -> std::vector<UpdateRange>
{
    auto out = std::vector<UpdateRange>{};
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
            out.emplace_back(start, end);
            start = nextEnd;
            end = start + 1;
        }
    }

    out.emplace_back(start, end);
    m_dirty.clear();
    return out;
}
} // namespace nc::graphics
