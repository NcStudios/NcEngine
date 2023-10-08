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
        explicit AnyComponent() noexcept = default;

        /**
         * @brief Construct an AnyComponent wrapper around a component.
         * @tparam T The underlying component type.
         * @param obj A pointer to the component instance.
         * @param handlers A pointer to the handler for T.
         */
        template<PooledComponent T>
        explicit AnyComponent(T* instance, ComponentHandler<T>* handler)
            : m_impl{std::make_unique<detail::AnyImplConcrete<T>>(instance, handler)}
        {
        }

        /** @brief Check if this refers to a valid component. */
        explicit operator bool() const noexcept
        {
            return static_cast<bool>(m_impl);
        }

        /** @brief Get the value of ComponentHandler::name for the concrete component type. */
        auto Name() const -> std::string_view
        {
            NC_ASSERT(m_impl, "Invalid use of null AnyComponent.");
            return m_impl->Name();
        }

        /** @brief Check if ComponentHandler::drawUI is set for the concrete component type. */
        auto HasDrawUI() const -> bool
        {
            NC_ASSERT(m_impl, "Invalid use of null AnyComponent.");
            return m_impl->HasDrawUI();
        }

        /** @brief Invoke ComponentHandler::drawUI with the component instance */
        void DrawUI()
        {
            NC_ASSERT(m_impl, "Invalid use of null AnyComponent.");
            m_impl->DrawUI();
        }

    private:
        /** @todo #438 construct in-place */
        std::unique_ptr<detail::AnyImplBase> m_impl;
};
} // namespace nc
