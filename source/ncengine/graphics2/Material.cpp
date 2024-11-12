#include "ncengine/graphics/Material.h"
#include "frontend/subsystem/MaterialRegistry.h"

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc& desc)
    : m_handle{s_registry->CreateInstance(desc)}
{
}

MaterialInstance::~MaterialInstance() noexcept
{
    if (m_handle != NullMaterialInstanceHandle)
    {
        s_registry->DestroyInstance(m_handle);
    }
}

auto MaterialInstance::Clone() const -> MaterialInstance
{
    return MaterialInstance{s_registry->GetInstanceDesc(m_handle)};
}

auto MaterialInstance::GetDesc() const -> const MaterialDesc&
{
    return s_registry->GetInstanceDesc(m_handle);
}

void MaterialInstance::SetDesc(const MaterialDesc& desc)
{
    s_registry->SetInstanceDesc(m_handle, desc);
}

void MaterialInstance::SetName(std::string_view name)
{
    s_registry->SetInstanceName(m_handle, name);
}
} // namespace nc
