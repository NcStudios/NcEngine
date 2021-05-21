#pragma once

#include <limits>
#include <cstdint>

namespace nc
{
    class EntityHandle
    {
        public:
            using Handle_t = uint32_t;

            explicit constexpr EntityHandle(Handle_t value) : m_value{value} {}
            explicit constexpr operator Handle_t() const { return m_value; }
            friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;
            static constexpr EntityHandle Invalid() { return EntityHandle{std::numeric_limits<Handle_t>::max()}; }

            struct Hash
            {
                size_t operator()(const EntityHandle& handle) const
                {
                    return static_cast<size_t>(static_cast<Handle_t>(handle));
                }
            };

        private:
            static constexpr Handle_t s_nullValue = 0u;
            Handle_t m_value;
    };

    // struct HandleTraits
    // {
    //     using handle_type = uint64_t;
    //     using index_type = uint32_t;
    //     using version_type = uint16_t;
    //     using flags_type = uint16_t;

    //     static constexpr handle_type null_handle = std::numeric_limits<handle_type>::max();
    //     static constexpr size_t flags_shift = 48u;
    //     static constexpr size_t version_shift = 32u;
    // };

    // class EntityHandle
    // {
    //     static constexpr auto Join(HandleTraits::index_type index,
    //                             HandleTraits::version_type version,
    //                             HandleTraits::flags_type flags)
    //     {
    //         return (HandleTraits::handle_type)flags << HandleTraits::flags_shift |
    //             (HandleTraits::handle_type)version << HandleTraits::version_shift |
    //             (HandleTraits::handle_type)index;
    //     }


    //     public:
    //         explicit constexpr EntityHandle(HandleTraits::index_type index,
    //                                         HandleTraits::version_type version,
    //                                         HandleTraits::flags_type flags)
    //             : m_handle{Join(index, version, flags)}
    //         {
    //         }

    //         constexpr void UpdateVersion() noexcept
    //         {
    //             m_handle = Join(Index(), Version() + 1u, Flags());
    //         }

    //         [[nodiscard]] constexpr auto Index() const noexcept -> HandleTraits::index_type
    //         {
    //             return static_cast<HandleTraits::index_type>(m_handle);
    //         }

    //         [[nodiscard]] constexpr auto Version() const noexcept -> HandleTraits::version_type
    //         {
    //             return static_cast<HandleTraits::version_type>(m_handle >> HandleTraits::version_shift);
    //         }

    //         [[nodiscard]] constexpr auto Flags() const noexcept -> HandleTraits::flags_type
    //         {
    //             return static_cast<HandleTraits::flags_type>(m_handle >> HandleTraits::flags_shift);
    //         }

    //         friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;

    //     private:
    //         HandleTraits::handle_type m_handle;
    // };
}