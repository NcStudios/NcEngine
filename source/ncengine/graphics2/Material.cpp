#include "ncengine/graphics/Material.h"
#include "frontend/subsystem/MaterialRegistry.h"

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc& desc)
    : m_handle{s_registry->CreateInstance(desc)}
{
}

auto MaterialInstance::Clone() const -> MaterialInstance
{
    return MaterialInstance{s_registry->GetInstanceDesc(m_handle)};
}

auto MaterialInstance::GetName() const -> std::string_view
{
    return s_registry->GetInstanceDesc(m_handle).name;
}

void MaterialInstance::SetName(std::string_view name)
{
    s_registry->SetInstanceName(m_handle, name);
}

auto MaterialInstance::GetPasses() const -> MaterialPasses
{
    return s_registry->GetInstanceDesc(m_handle).passes;
}

auto MaterialInstance::GetProperties() const -> const MaterialProperties&
{
    return s_registry->GetInstanceDesc(m_handle).properties;
}

void MaterialInstance::SetProperties(const MaterialProperties& desc)
{
    s_registry->SetInstanceProperties(m_handle, desc);
}

void MaterialInstance::Release() noexcept
{
    if (m_handle != NullMaterialInstanceHandle)
    {
        s_registry->DestroyInstance(m_handle);
    }
}
} // namespace nc
