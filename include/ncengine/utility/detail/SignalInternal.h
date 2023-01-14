#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace nc::detail
{
/** A link from a connection back to a signal. Directly pointing to a signal would
 *  require guaranteeing pointer stability. To avoid the burden of non-moveable
 *  signals, each signal allocates one of these to use as a contact point. Removals
 *  are then accrued here, and written back to slot storage prior to calling Emit().
*/
class ConnectionBacklink
{
    public:
        ConnectionBacklink() = default;
        ConnectionBacklink(const ConnectionBacklink&) = delete;
        ConnectionBacklink(ConnectionBacklink&&) = delete;
        ConnectionBacklink& operator=(const ConnectionBacklink&) = delete;
        ConnectionBacklink& operator=(ConnectionBacklink&&) = delete;

        auto HasPendingDisconnections() const noexcept -> bool
        {
            return !m_pending.empty();
        }

        auto GetPendingDisconnections() -> std::vector<int>
        {
            auto lock = std::lock_guard{m_mutex};
            return std::exchange(m_pending, std::vector<int>{});
        }

        void StageDisconnect(int id)
        {
            auto lock = std::lock_guard{m_mutex};
            m_pending.push_back(id);
        }

    private:
        std::vector<int> m_pending;
        std::mutex m_mutex;
};

template<class... Args>
class SharedConnectionState
{
    public:
        SharedConnectionState(ConnectionBacklink* link, int id) noexcept
            : m_link{link}, m_id{id}, m_connected{true}
        {
        }

        auto Id() const noexcept -> int
        {
            return m_id;
        }

        auto IsConnected() const noexcept -> bool
        {
            return m_connected;
        }

        auto Disconnect() noexcept -> bool
        {
            const auto connected = std::exchange(m_connected, false);
            if (connected)
            {
                m_link->StageDisconnect(m_id);
            }

            return connected;
        }

    private:
        ConnectionBacklink* m_link;
        int m_id;
        bool m_connected;
};

template<class... Args>
class Slot
{
    public:
        using SharedConnectionState_t = SharedConnectionState<Args...>;

        Slot(std::function<void(Args...)> func, ConnectionBacklink* link, size_t priority, int id)
            : m_func{std::move(func)},
              m_state{std::make_shared<SharedConnectionState_t>(link, id)},
              m_priority{priority},
              m_id{id}
        {
        }

        auto Id() const noexcept -> int
        {
            return m_id;
        }

        auto Priority() const noexcept -> size_t
        {
            return m_priority;
        }

        void operator()(Args... args) const
        {
            m_func(args...);
        }

        auto GetState() const -> std::weak_ptr<SharedConnectionState_t>
        {
            return m_state;
        }

    private:
        std::function<void(Args...)> m_func;
        std::shared_ptr<SharedConnectionState_t> m_state;
        size_t m_priority;
        int m_id;
};
} // namespace nc::internal
