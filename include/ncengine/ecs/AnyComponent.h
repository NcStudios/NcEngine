#pragma once

#include "detail/AnyComponentUtility.h"

#include "ncutility/NcError.h"

namespace nc
{
/** @brief Generic interface around component types. */
class AnyComponent
{
    public:
        /** @brief Construct a null AnyComponent. */
        constexpr explicit AnyComponent() noexcept
            : m_storage{} {}

        /**
         * @brief Construct an AnyComponent wrapper around a component.
         * @tparam T The underlying component type.
         * @param obj A valid pointer to the component instance.
         * @param handlers A valid pointer to the handler for T.
         * @throw NcError if either the instance or handler are null.
         */
        template<PooledComponent T>
        explicit AnyComponent(T* instance, ComponentHandler<T>* handler)
            : m_storage{instance, handler} {}

        /** @brief Check if this refers to a valid component. */
        explicit operator bool() const noexcept
        {
            return m_storage.HasValue();
        }

        /** @brief Check if two AnyComponents refer to the same component. */
        auto operator==(const AnyComponent& other) const noexcept -> bool
        {
            return m_storage == other.m_storage;
        }

        /** @brief Check if two AnyComponents refer to different components. */
        auto operator!=(const AnyComponent& other) const noexcept -> bool
        {
            return !(*this == other);
        }

        /**
         * @brief Get the value of ComponentHandler::name for the concrete component type.
         * @throw NcError if invoked on a null AnyComponent.
         */
        auto Name() const -> std::string_view
        {
            NC_ASSERT(m_storage.HasValue(), "Invalid use of null AnyComponent.");
            return m_storage.AsImpl()->Name();
        }

        /**
         * @brief Check if ComponentHandler::drawUI is set for the concrete component type.
         * @throw NcError if invoked on a null AnyComponent.
         */
        auto HasDrawUI() const -> bool
        {
            NC_ASSERT(m_storage.HasValue(), "Invalid use of null AnyComponent.");
            return m_storage.AsImpl()->HasDrawUI();
        }

        /**
         * @brief Invoke ComponentHandler::drawUI with the component instance.
         * @throw NcError if invoked on a null AnyComponent.
         */
        void DrawUI()
        {
            NC_ASSERT(m_storage.HasValue(), "Invalid use of null AnyComponent.");
            m_storage.AsImpl()->DrawUI();
        }

    private:
        detail::AnyImplStorage m_storage;
};
} // namespace nc
