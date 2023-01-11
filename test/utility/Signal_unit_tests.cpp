#include "gtest/gtest.h"
#include "utility/Signal.h"

namespace
{
struct Foo
{
    void Test(int v) { value += v; }
    void ConstTest(int v) const { mutableValue += v; }
    int value = 0;
    mutable int mutableValue = 0;
};
}

/** Connection Tests */
TEST(Signal_unit_tests, Connection_IsConnected_ValidConnection_ReturnsTrue)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_IsConnected_InvalidConnection_ReturnsFalse)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    state->Disconnect();
    auto connection = nc::Connection<int>{state};
    EXPECT_FALSE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_Disconnect_ValidConnection_ReturnsTrue)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    EXPECT_TRUE(connection.Disconnect());
}

TEST(Signal_unit_tests, Connection_Disconnect_InvalidConnection_ReturnsFalse)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    state->Disconnect();
    auto connection = nc::Connection<int>{state};
    EXPECT_FALSE(connection.Disconnect());
}

TEST(Signal_unit_tests, Connection_Disconnect_RemovesConnection)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    connection.Disconnect();
    EXPECT_FALSE(state->IsConnected());
    EXPECT_FALSE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_Destructor_ValidConnection_RemovesConnection)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    {
        auto connection = nc::Connection<int>{state};
        EXPECT_TRUE(state->IsConnected());
        EXPECT_TRUE(connection.IsConnected());
    }
    EXPECT_FALSE(state->IsConnected());
}

TEST(Signal_unit_tests, Connection_Destructor_InvalidConnection_DoesNotAttemptDisconnect)
{
    auto manager = std::make_shared<nc::detail::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::detail::SharedConnectionState<int>>(manager.get(), id);
    {
        auto connection = nc::Connection<int>{state};
        EXPECT_TRUE(state->IsConnected());
        EXPECT_TRUE(connection.IsConnected());
        connection.Disconnect();
    }
    // Just verifying nothing explodes
}

/** Signal Tests */
TEST(Signal_unit_tests, Signal_Connect_Lambda_ReturnsValidConnection)
{
    auto signal = nc::Signal<int>{};
    auto connection = signal.Connect([](int){});
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_MemberFunction_ReturnsValidConnection)
{
    auto signal = nc::Signal<int>{};
    auto foo = Foo{};
    auto connection = signal.Connect(&foo, &Foo::Test);
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_ConstMemberFunction_ReturnsValidConnection)
{
    auto signal = nc::Signal<int>{};
    auto foo = Foo{};
    const auto& constFoo = foo;
    auto connection = signal.Connect(&constFoo, &Foo::ConstTest);
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_UpdatesConnectionCount)
{
    auto signal = nc::Signal<int>{};
    EXPECT_EQ(signal.ConnectionCount(), 0);
    auto connection = signal.Connect([](int){});
    EXPECT_EQ(signal.ConnectionCount(), 1);
}

TEST(Signal_unit_tests, Signal_DisconnectAll_ClearsConnections)
{
    auto signal = nc::Signal<int>{};
    auto calls = 0;
    auto connection = signal.Connect([&calls](int v){calls += v;});
    signal.DisconnectAll();
    signal.Emit(1);
    EXPECT_EQ(calls, 0);
}

TEST(Signal_unit_tests, Signal_Emit_InvokesConnections)
{
    auto signal = nc::Signal<int>{};
    auto calls = 0;
    auto connection1 = signal.Connect([&calls](int v){calls += v;});
    auto connection2 = signal.Connect([&calls](int v){calls += v;});
    signal.Emit(1);
    EXPECT_EQ(calls, 2);
}

TEST(Signal_unit_tests, Signal_Emit_MultiplePriorities_CallsInOrder)
{
    static constexpr auto minPriority = 0ull;
    static constexpr auto lowPriority = 1ull;
    static constexpr auto highPriority = 2ull;
    auto callOrder = std::vector<size_t>{};
    auto minPriorityFunc = [&callOrder](int){ callOrder.push_back(minPriority); };
    auto lowPriorityFunc = [&callOrder](int){ callOrder.push_back(lowPriority); };
    auto highPriorityFunc = [&callOrder](int){ callOrder.push_back(highPriority); };
    auto signal = nc::Signal<int>{};

    {
        auto connection1 = signal.Connect(minPriorityFunc, minPriority);
        auto connection2 = signal.Connect(lowPriorityFunc, lowPriority);
        auto connection3 = signal.Connect(highPriorityFunc, highPriority);
        signal.Emit(0);
        ASSERT_EQ(callOrder.size(), 3);
        EXPECT_EQ(callOrder[0], highPriority);
        EXPECT_EQ(callOrder[1], lowPriority);
        EXPECT_EQ(callOrder[2], minPriority);
    }

    callOrder.clear();
    signal.DisconnectAll();

    {
        auto connection1 = signal.Connect(highPriorityFunc, highPriority);
        auto connection2 = signal.Connect(lowPriorityFunc, lowPriority);
        auto connection3 = signal.Connect(minPriorityFunc, minPriority);
        signal.Emit(0);
        ASSERT_EQ(callOrder.size(), 3);
        EXPECT_EQ(callOrder[0], highPriority);
        EXPECT_EQ(callOrder[1], lowPriority);
        EXPECT_EQ(callOrder[2], minPriority);
    }

    callOrder.clear();
    signal.DisconnectAll();

    {
        auto connection1 = signal.Connect(highPriorityFunc, highPriority);
        auto connection2 = signal.Connect(minPriorityFunc, minPriority);
        auto connection3 = signal.Connect(lowPriorityFunc, lowPriority);
        auto connection4 = signal.Connect(highPriorityFunc, highPriority);
        auto connection5 = signal.Connect(highPriorityFunc, highPriority);
        auto connection6 = signal.Connect(lowPriorityFunc, lowPriority);
        auto connection7 = signal.Connect(minPriorityFunc, minPriority);
        signal.Emit(0);
        ASSERT_EQ(callOrder.size(), 7);
        EXPECT_EQ(callOrder[0], highPriority);
        EXPECT_EQ(callOrder[1], highPriority);
        EXPECT_EQ(callOrder[2], highPriority);
        EXPECT_EQ(callOrder[3], lowPriority);
        EXPECT_EQ(callOrder[4], lowPriority);
        EXPECT_EQ(callOrder[5], minPriority);
        EXPECT_EQ(callOrder[6], minPriority);
    }
}

TEST(Signal_unit_tests, Signal_Emit_AfterSignalSideDisconnect_DoesNothing)
{
    auto signal = nc::Signal<int>{};
    auto calls = 0;
    auto connection = signal.Connect([&calls](int v){calls += v;});
    signal.DisconnectAll();
    signal.Emit(1);
    EXPECT_EQ(calls, 0);
}

TEST(Signal_unit_tests, Signal_Emit_AfterConnectionSideDisconnect_DoesNothing)
{
    auto signal = nc::Signal<int>{};
    auto calls = 0;
    auto connection = signal.Connect([&calls](int v){calls += v;});
    connection.Disconnect();
    signal.Emit(1);
    EXPECT_EQ(calls, 0);
}

TEST(Signal_unit_tests, Signal_Destructor_UpdatesSharedConnectionState)
{
    auto signal = std::make_unique<nc::Signal<int>>();
    auto connection1 = signal->Connect([](int){});
    auto connection2 = signal->Connect([](int){});
    EXPECT_TRUE(connection1.IsConnected());
    EXPECT_TRUE(connection2.IsConnected());
    signal = nullptr;
    EXPECT_FALSE(connection1.IsConnected());
    EXPECT_FALSE(connection2.IsConnected());
}
