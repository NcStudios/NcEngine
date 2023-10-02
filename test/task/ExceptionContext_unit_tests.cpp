#include "gtest/gtest.h"
#include "task/ExceptionContext.h"

#include <exception>

TEST(ExceptionContextTests, ThrowIfExceptionStored_noException_succeeds)
{
    auto uut = nc::task::ExceptionContext{};
    EXPECT_NO_THROW(uut.ThrowIfExceptionStored());
}

TEST(ExceptionContextTests, ThrowIfExceptionStored_hasException_rethrows)
{
    auto uut = nc::task::ExceptionContext{};

    try
    {
        throw std::logic_error{"barf"};
    }
    catch(const std::exception&)
    {
        uut.StoreException(std::current_exception());
    }

    EXPECT_THROW(uut.ThrowIfExceptionStored(), std::logic_error);
    EXPECT_NO_THROW(uut.ThrowIfExceptionStored()); // first call clears exception
}

TEST(ExceptionContextTests, StoreException_multipleExceptions_ignoresAllButFirst)
{
    auto uut = nc::task::ExceptionContext{};

    try
    {
        throw std::logic_error{"barf"};
    }
    catch(const std::exception&)
    {
        uut.StoreException(std::current_exception());
    }

    try
    {
        throw std::runtime_error{"ignore me"};
    }
    catch(const std::exception&)
    {
        uut.StoreException(std::current_exception());
    }

    EXPECT_THROW(uut.ThrowIfExceptionStored(), std::logic_error);
}

TEST(ExceptionContextTests, Guard_noexceptFunc_doesNotWrapFunc)
{
    auto ctx = nc::task::ExceptionContext{};
    auto expected = []() noexcept(true) {};
    auto actual = nc::task::Guard(ctx, expected);

    static_assert(std::same_as<decltype(expected), decltype(actual)>);
}

TEST(ExceptionContextTests, Guard_potentiallyThrowingFunc_wrapsFunc)
{
    auto ctx = nc::task::ExceptionContext{};
    auto expected = []() noexcept(false) {};
    auto actual = nc::task::Guard(ctx, expected);

    static_assert(!std::same_as<decltype(expected), decltype(actual)>);
}

TEST(ExceptionContextTests, Guard_functionThrows_storesException)
{
    auto ctx = nc::task::ExceptionContext{};
    auto wrapped = nc::task::Guard(ctx, []() { throw std::runtime_error{"boom"}; });
    wrapped();
    EXPECT_THROW(ctx.ThrowIfExceptionStored(), std::runtime_error);
}
