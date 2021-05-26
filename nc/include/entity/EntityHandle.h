#pragma once

#include <limits>
#include <cstdint>

namespace nc
{
    struct HandleTraits
    {
        using handle_type = uint64_t;
        using index_type = uint32_t;
        using version_type = uint16_t;
        using layer_type = uint8_t;
        using flags_type = uint8_t;

        static constexpr handle_type null_handle = std::numeric_limits<handle_type>::max();
        static constexpr size_t flags_shift = 56u;
        static constexpr size_t layer_shift = 48u;
        static constexpr size_t version_shift = 32u;
    };

    struct EntityFlags
    {
        static constexpr HandleTraits::flags_type None = 0u;
        static constexpr HandleTraits::flags_type Static = 1u;
    };


    /** @todo Maybe move some of these functions so only the engine can see them. I 
     *  don't think game logic should every care about handle details. */

    class EntityHandle
    {
        static constexpr auto Join(HandleTraits::index_type index,
                                   HandleTraits::version_type version,
                                   HandleTraits::layer_type layer,
                                   HandleTraits::flags_type flags)
        {
            return (HandleTraits::handle_type)flags << HandleTraits::flags_shift |
                   (HandleTraits::handle_type)layer << HandleTraits::layer_shift |
                   (HandleTraits::handle_type)version << HandleTraits::version_shift |
                   (HandleTraits::handle_type)index;
        }

        public:
            static constexpr EntityHandle Null()
            {
                return EntityHandle{HandleTraits::null_handle};
            }

            explicit constexpr EntityHandle(HandleTraits::index_type index,
                                            HandleTraits::version_type version,
                                            HandleTraits::layer_type layer,
                                            HandleTraits::flags_type flags)
                : m_handle{Join(index, version, layer, flags)}
            {
            }

            explicit constexpr EntityHandle(HandleTraits::handle_type handle)
                : m_handle{handle}
            {
            }

            explicit constexpr operator HandleTraits::handle_type() const
            {
                return m_handle;
            }

            [[nodiscard]] constexpr bool Valid() const
            {
                return m_handle != HandleTraits::null_handle;
            }

            // can do better
            constexpr void SetVersion(HandleTraits::version_type version) noexcept
            {
                m_handle = Join(Index(), version, Layer(), Flags());
            }

            constexpr void Recycle(HandleTraits::layer_type layer, HandleTraits::flags_type flags) noexcept
            {
                m_handle = Join(Index(), Version() + 1u, layer, flags);
            }

            [[nodiscard]] constexpr auto Index() const noexcept -> HandleTraits::index_type
            {
                return static_cast<HandleTraits::index_type>(m_handle);
            }

            [[nodiscard]] constexpr auto Version() const noexcept -> HandleTraits::version_type
            {
                return static_cast<HandleTraits::version_type>(m_handle >> HandleTraits::version_shift);
            }

            [[nodiscard]] constexpr auto Layer() const noexcept -> HandleTraits::layer_type
            {
                return static_cast<HandleTraits::layer_type>(m_handle >> HandleTraits::layer_shift);
            }

            [[nodiscard]] constexpr auto Flags() const noexcept -> HandleTraits::flags_type
            {
                return static_cast<HandleTraits::flags_type>(m_handle >> HandleTraits::flags_shift);
            }

            [[nodiscard]] constexpr bool IsStatic() const noexcept
            {
                return static_cast<bool>(Flags() & EntityFlags::Static);
            }

            friend auto constexpr operator<=>(const EntityHandle&, const EntityHandle&) = default;

        private:
            HandleTraits::handle_type m_handle;
    };
}