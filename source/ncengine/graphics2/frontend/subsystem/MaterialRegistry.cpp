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
        .diffuseTexIndex = properties.diffuseTex.index,
        .gradientEnd = properties.gradientEnd,
        .normalTexIndex = properties.normalTex.index,
        .hatchTexIndex = properties.hatchTex.index,
        .normalIntensity = properties.normalIntensity,
        .hatchTiling = properties.hatchTiling,
        .gradientAmount = properties.gradientAmount,
        .reflectivity = properties.reflectivity,
        .useTextureNormals = properties.useTextureNormals,
        .useFlatShading = properties.useFlatShading
    };
}
} // anonymous namespace

namespace nc::graphics
{
MaterialRegistry::MaterialRegistry(uint32_t maxInstances)
    : m_buffer{maxInstances}
{
    MaterialInstance::s_registry = this;
}

auto MaterialRegistry::CreateInstance(const MaterialDesc& desc) -> MaterialInstanceHandle
{
    const auto index = m_buffer.GetStagingArea().Emplace(ToMaterialData(desc.properties));
    if (index >= m_descriptions.size())
    {
        m_descriptions.push_back(desc);
    }
    else
    {
        m_descriptions[index] = desc;
    }

    return index;
}

void MaterialRegistry::DestroyInstance(MaterialInstanceHandle index)
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    m_descriptions[index] = MaterialDesc{};
    m_buffer.GetStagingArea().Erase(index);
}

auto MaterialRegistry::GetInstanceDesc(MaterialInstanceHandle index) const -> const MaterialDesc&
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    return m_descriptions[index];
}

void MaterialRegistry::SetInstanceProperties(MaterialInstanceHandle index, const MaterialProperties& properties)
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    m_buffer.GetStagingArea().Update(index, ToMaterialData(properties));
    m_descriptions[index].properties = properties;
}

auto MaterialRegistry::GetInstanceData(MaterialInstanceHandle index) const -> const MaterialData&
{
    NC_ASSERT(index < m_buffer.size(), "Invalid MaterialInstanceHandle");
    return m_buffer.AccessForRead(index);
}

void MaterialRegistry::SetInstanceName(MaterialInstanceHandle index, std::string_view name)
{
    NC_ASSERT(index < m_descriptions.size(), "Invalid MaterialInstanceHandle");
    m_descriptions[index].name = std::string{name};
}

auto MaterialRegistry::BuildState() -> BufferUpdateInfo<MaterialData>
{
    m_buffer.CommitPendingChanges();
    return m_buffer.BuildUpdateInfo();
}
} // namespace nc::graphics
