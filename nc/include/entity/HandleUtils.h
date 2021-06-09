#pragma once

#include "EntityHandle.h"

namespace nc
{
    struct HandleUtils
    {
        [[nodiscard]] static constexpr auto Index(EntityHandle handle) noexcept -> HandleTraits::index_type;
        [[nodiscard]] static constexpr auto Version(EntityHandle handle) noexcept -> HandleTraits::version_type;
        [[nodiscard]] static constexpr auto Layer(EntityHandle handle) noexcept -> HandleTraits::layer_type;
        [[nodiscard]] static constexpr auto Flags(EntityHandle handle) noexcept -> HandleTraits::flags_type;
        [[nodiscard]] static constexpr bool IsStatic(EntityHandle handle) noexcept;
        [[nodiscard]] static constexpr auto SetVersion(EntityHandle handle, HandleTraits::version_type version) noexcept -> EntityHandle;
        [[nodiscard]] static constexpr auto Recycle(EntityHandle handle, HandleTraits::layer_type layer, HandleTraits::flags_type flags) noexcept -> EntityHandle;
        [[nodiscard]] static constexpr auto Join(HandleTraits::index_type index,
                                                 HandleTraits::version_type version,
                                                 HandleTraits::layer_type layer,
                                                 HandleTraits::flags_type flags) noexcept -> HandleTraits::handle_type;
    };

    // can do better
    constexpr auto HandleUtils::SetVersion(EntityHandle handle, HandleTraits::version_type version) noexcept -> EntityHandle
    {
        handle.m_handle = Join(Index(handle), version, Layer(handle), Flags(handle));
        return handle;
    }

    constexpr auto HandleUtils::Recycle(EntityHandle handle, HandleTraits::layer_type layer, HandleTraits::flags_type flags) noexcept -> EntityHandle
    {
        handle.m_handle = Join(Index(handle), Version(handle) + 1u, layer, flags);
        return handle;
    }

    constexpr auto HandleUtils::Index(EntityHandle handle) noexcept -> HandleTraits::index_type
    {
        return static_cast<HandleTraits::index_type>(handle.m_handle);
    }

    constexpr auto HandleUtils::Version(EntityHandle handle) noexcept -> HandleTraits::version_type
    {
        return static_cast<HandleTraits::version_type>(handle.m_handle >> HandleTraits::VersionShift);
    }

    constexpr auto HandleUtils::Layer(EntityHandle handle) noexcept -> HandleTraits::layer_type
    {
        return static_cast<HandleTraits::layer_type>(handle.m_handle >> HandleTraits::LayerShift);
    }

    constexpr auto HandleUtils::Flags(EntityHandle handle) noexcept -> HandleTraits::flags_type
    {
        return static_cast<HandleTraits::flags_type>(handle.m_handle >> HandleTraits::FlagsShift);
    }

    constexpr bool HandleUtils::IsStatic(EntityHandle handle) noexcept
    {
        return static_cast<bool>(Flags(handle) & EntityFlags::Static);
    }

    constexpr auto HandleUtils::Join(HandleTraits::index_type index,
                                     HandleTraits::version_type version,
                                     HandleTraits::layer_type layer,
                                     HandleTraits::flags_type flags) noexcept -> HandleTraits::handle_type
    {
        return static_cast<HandleTraits::handle_type>(flags)   << HandleTraits::FlagsShift   |
               static_cast<HandleTraits::handle_type>(layer)   << HandleTraits::LayerShift   |
               static_cast<HandleTraits::handle_type>(version) << HandleTraits::VersionShift |
               static_cast<HandleTraits::handle_type>(index);
    }
}