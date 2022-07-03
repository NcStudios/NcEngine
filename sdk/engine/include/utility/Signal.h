#pragma once

#include "debug/Utils.h"
#include "internal/SignalInternal.h"
#include <unordered_map>

namespace nc
{
template<class... Args>
class Connection
{
    public:
        using ConnectionState_t = internal::SharedConnectionState<Args...>;

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

template<class... Args>
class Signal
{
    public:
        using Slot_t = internal::Slot<Args...>;
        using Connection_t = Connection<Args...>;

        Signal() = default;
        Signal(Signal&&) = default;
        Signal& operator=(Signal&&) = default;
        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;

        /** Connect a std::function */
        [[nodiscard]] auto Connect(std::function<void(Args...)> func) -> Connection_t
        {
            const auto id = ++m_currentId;
            auto [it, result] = m_slots.emplace(id, Slot_t{std::move(func), m_link.get(), id});
            NC_ASSERT(result, "Signal::Connect failure");
            return Connection_t{it->second.GetState()};
        }

        /** Connect a member function */
        template<class T>
        [[nodiscard]] auto Connect(T *inst, void (T::*func)(Args...)) -> Connection_t
        {
            return Connect([=](Args... args){ (inst->*func)(args...); });
        }

        /** Connect a const member function */
        template<class T>
        [[nodiscard]] auto Connect(const T *inst, void (T::*func)(Args...) const) -> Connection_t
        {
            return Connect([=](Args... args){ (inst->*func)(args...); });
        }

        /** Remove all connections */
        void DisconnectAll()
        {
            m_link->GetPendingDisconnections(); // Don't need Sync() - just discard
            m_slots.clear();
            m_currentId = 0;
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
            for (const auto& [id, slot] : m_slots)
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
        mutable std::unique_ptr<internal::ConnectionBacklink> m_link = std::make_unique<internal::ConnectionBacklink>();
        mutable std::unordered_map<int, Slot_t> m_slots{};
        int m_currentId{0};

        void Sync() const
        {
            if (m_link->HasPendingDisconnections())
            {
                auto pendingRemovals = m_link->GetPendingDisconnections();
                for(auto id : pendingRemovals)
                {
                    m_slots.erase(id);
                }
            }
        }
};
} // namespace nc
