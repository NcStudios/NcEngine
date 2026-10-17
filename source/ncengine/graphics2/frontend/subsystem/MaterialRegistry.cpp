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
        .gradientEnd = properties.gradientEnd,
        .primaryColor = properties.primaryColor,
        .secondaryColor = properties.secondaryColor,
        .tertiaryColor = properties.tertiaryColor,
        .stencilOutlineColor = properties.stencilOutlineColor,
        .normalIntensity = properties.normalIntensity,
        .hatchTiling = properties.hatchTiling,
        .gradientAmount = properties.gradientAmount,
        .reflectivity = properties.reflectivity,
        .stencilOutlineWidth = properties.stencilOutlineWidth,
        .diffuseTexIndex = properties.diffuseTex.index,
        .normalTexIndex = properties.normalTex.index,
        .hatchTexIndex = properties.hatchTex.index,
        .useTextureNormals = properties.useTextureNormals,
        .useFlatShading = properties.useFlatShading,
        .useColorOverride = properties.useColorOverride,
        .useHatchTexture = properties.useHatchTexture,
        .useStencilOutline = properties.useStencilOutline
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

auto MaterialRegistry::CreateInstance(MaterialDesc desc) -> MaterialInstanceHandle
{
    if (desc.properties.useStencilOutline)
    {
        desc.passes &= ~MaterialPassFlag::Toon;
        if (!(desc.passes & MaterialPassFlag::ToonStencilWrite))
        {
            desc.passes |= MaterialPassFlag::ToonStencilWrite;
        }

        if (!(desc.passes & MaterialPassFlag::StencilOutline))
        {
            desc.passes |= MaterialPassFlag::StencilOutline;
        }
    }
    else
    {
        desc.passes &= ~MaterialPassFlag::ToonStencilWrite;
        desc.passes &= ~MaterialPassFlag::StencilOutline;
        if (!(desc.passes & MaterialPassFlag::Toon))
        {
            desc.passes |= MaterialPassFlag::Toon;
        }
    }

    const auto index = m_buffer.GetStagingArea().Emplace(ToMaterialData(desc.properties));
    if (index >= m_descriptions.size())
    {
        m_descriptions.push_back(std::move(desc));
    }
    else
    {
        m_descriptions[index] = std::move(desc);
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

    const auto oldPasses = m_descriptions[index].passes;
    auto newPasses = oldPasses;

    if (properties.useStencilOutline)
    {
        newPasses &= ~MaterialPassFlag::Toon;
        newPasses |= MaterialPassFlag::ToonStencilWrite;
        newPasses |= MaterialPassFlag::StencilOutline;
    }
    else
    {
        newPasses &= ~MaterialPassFlag::ToonStencilWrite;
        newPasses &= ~MaterialPassFlag::StencilOutline;
        newPasses |= MaterialPassFlag::Toon;
    }

    m_buffer.GetStagingArea().Update(index, ToMaterialData(properties));
    m_descriptions[index].properties = properties;
    m_descriptions[index].passes = newPasses;

    if (newPasses != oldPasses)
    {
        m_onPassesChanged.Emit(index, oldPasses);
    }
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
