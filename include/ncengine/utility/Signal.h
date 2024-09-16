#pragma once

#include "detail/SignalInternal.h"

#include "ncutility/NcError.h"

#include <algorithm>

namespace nc
{
/** @brief An RAII object managing a Signal connection.
 * 
 *  When a Connection goes out of scope or Disconnect() is explicitly called,
 *  the associated function will be removed from the Signal, if the Signal
 *  still exists. Signals and Connections may be destroyed in any order.
 */
template<class... Args>
class Connection
{
    public:
        using ConnectionState_t = detail::SharedConnectionState<Args...>;

        explicit Connection(std::weak_ptr<ConnectionState_t> state) noexcept
            : m_state{std::move(state)}
        {
        }

        ~Connection() noexcept
        {
            Disconnect();
        }

        Connection(Connection&&) = default;
        Connection& operator=(Connection&&) = default;
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        bool IsConnected() const noexcept
        {
            const auto state = m_state.lock();
            return state && state->IsConnected();
        }

        bool Disconnect() noexcept
        {
            auto state = m_state.lock();
            return state && state->Disconnect();
        }

    private:
        std::weak_ptr<ConnectionState_t> m_state;
};

/** @brief Priority values for controlling call order from a Signal. */
struct SignalPriority
{
    static constexpr size_t Lowest = 0ull;
    static constexpr size_t Highest = std::numeric_limits<size_t>::max();
};

/** @brief An event source supporting multiple Connections. */
template<class... Args>
class Signal
{
    public:
        using Slot_t = detail::Slot<Args...>;
        using Connection_t = Connection<Args...>;

        Signal() = default;
        Signal(Signal&&) = default;
        Signal& operator=(Signal&&) = default;
        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;

        /** Connect a std::function */
        [[nodiscard]] auto Connect(std::function<void(Args...)> func, size_t priority = SignalPriority::Highest) -> Connection_t
        {
            const auto pos = std::ranges::find_if(m_slots, [priority](auto&& slot)
            {
                return priority >= slot.Priority();
            });

            if (pos != m_slots.cend())
            {
                auto result = m_slots.emplace(pos, std::move(func), m_link.get(), priority, ++m_currentId);
                return Connection_t{result->GetState()};
            }

            auto& result = m_slots.emplace_back(std::move(func), m_link.get(), priority, ++m_currentId);
            return Connection_t{result.GetState()};
        }

        /** Connect a member function */
        template<class T>
        [[nodiscard]] auto Connect(T *inst, void (T::*func)(Args...), size_t priority = SignalPriority::Highest) -> Connection_t
        {
            return Connect([=](Args... args){ (inst->*func)(args...); }, priority);
        }

        /** Connect a const member function */
        template<class T>
        [[nodiscard]] auto Connect(const T *inst, void (T::*func)(Args...) const, size_t priority = SignalPriority::Highest) -> Connection_t
        {
            return Connect([=](Args... args){ (inst->*func)(args...); }, priority);
        }

        /** Remove all connections */
        void DisconnectAll()
        {
            m_slots.clear();
            m_currentId = 0;
            m_link->GetPendingDisconnections(); // Don't need Sync() - just discard
        }

        /** Get the number of active connections */
        auto ConnectionCount() const noexcept -> size_t
        {
            Sync();
            return m_slots.size();
        }

        /** Invoke all slots */
        void Emit(Args... args)
        {
            Sync();
            for (const auto& slot : m_slots)
            {
                slot(args...);
            }
        }

        /** Invoke all slots */
        void operator()(Args... args)
        {
            Emit(args...);
        }

    private:
        mutable std::unique_ptr<detail::ConnectionBacklink> m_link = std::make_unique<detail::ConnectionBacklink>();
        mutable std::vector<Slot_t> m_slots{};
        int m_currentId{0};

        void Sync() const
        {
            const auto pendingRemovals = m_link->GetPendingDisconnections();
            for(const auto id : pendingRemovals)
            {
                std::erase_if(m_slots, [id](const auto& state) { return state.Id() == id; });
            }
        }
};
} // namespace nc
