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
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_IsConnected_InvalidConnection_ReturnsFalse)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    state->Disconnect();
    auto connection = nc::Connection<int>{state};
    EXPECT_FALSE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_Disconnect_ValidConnection_ReturnsTrue)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    EXPECT_TRUE(connection.Disconnect());
}

TEST(Signal_unit_tests, Connection_Disconnect_InvalidConnection_ReturnsFalse)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    state->Disconnect();
    auto connection = nc::Connection<int>{state};
    EXPECT_FALSE(connection.Disconnect());
}

TEST(Signal_unit_tests, Connection_Disconnect_RemovesConnection)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    auto connection = nc::Connection<int>{state};
    connection.Disconnect();
    EXPECT_FALSE(state->IsConnected());
    EXPECT_FALSE(connection.IsConnected());
}

TEST(Signal_unit_tests, Connection_Destructor_ValidConnection_RemovesConnection)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
    {
        auto connection = nc::Connection<int>{state};
        EXPECT_TRUE(state->IsConnected());
        EXPECT_TRUE(connection.IsConnected());
    }
    EXPECT_FALSE(state->IsConnected());
}

TEST(Signal_unit_tests, Connection_Destructor_InvalidConnection_DoesNotAttemptDisconnect)
{
    auto manager = std::make_shared<nc::internal::ConnectionBacklink>();
    auto id = 1;
    auto state = std::make_shared<nc::internal::SharedConnectionState<int>>(manager.get(), id);
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
    nc::Signal<int> signal;
    auto connection = signal.Connect([](int){});
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_MemberFunction_ReturnsValidConnection)
{
    nc::Signal<int> signal;
    Foo foo;
    auto connection = signal.Connect(&foo, &Foo::Test);
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_ConstMemberFunction_ReturnsValidConnection)
{
    nc::Signal<int> signal;
    Foo foo;
    const Foo& constFoo = foo;
    auto connection = signal.Connect(&constFoo, &Foo::ConstTest);
    EXPECT_TRUE(connection.IsConnected());
}

TEST(Signal_unit_tests, Signal_Connect_UpdatesConnectionCount)
{
    nc::Signal<int> signal;
    EXPECT_EQ(signal.ConnectionCount(), 0);
    auto connection = signal.Connect([](int){});
    EXPECT_EQ(signal.ConnectionCount(), 1);
}

TEST(Signal_unit_tests, Signal_DisconnectAll_ClearsConnections)
{
    nc::Signal<int> signal;
    int calls = 0;
    auto connection = signal.Connect([&calls](int v){calls += v;});
    signal.DisconnectAll();
    signal.Emit(1);
    EXPECT_EQ(calls, 0);
}

TEST(Signal_unit_tests, Signal_Emit_InvokesConnections)
{
    nc::Signal<int> signal;
    int calls = 0;
    auto connection1 = signal.Connect([&calls](int v){calls += v;});
    auto connection2 = signal.Connect([&calls](int v){calls += v;});
    signal.Emit(1);
    EXPECT_EQ(calls, 2);
}

TEST(Signal_unit_tests, Signal_Emit_AfterSignalSideDisconnect_DoesNothing)
{
    nc::Signal<int> signal;
    int calls = 0;
    auto connection = signal.Connect([&calls](int v){calls += v;});
    signal.DisconnectAll();
    signal.Emit(1);
    EXPECT_EQ(calls, 0);
}

TEST(Signal_unit_tests, Signal_Emit_AfterConnectionSideDisconnect_DoesNothing)
{
    nc::Signal<int> signal;
    int calls = 0;
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

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}