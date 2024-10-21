#include "gtest/gtest.h"
#include "ncengine/utility/Log.h"
#include "ncengine/utility/FileLogger.h"

#include "ncutility/NcError.h"

#include <filesystem>
#include <fstream>

class LogTest : public testing::Test
{
    protected:
        void TearDown() override
        {
            nc::SetLogCallback(nc::detail::DefaultLogCallback);
        }
};

class FileLoggerTest : public testing::Test
{
    protected:
        static constexpr auto MaxMessageCount = 3ull;
        std::filesystem::path testLogPath = "";

        auto MakeUut() -> nc::FileLogger
        {
            return nc::FileLogger{testLogPath.string(), 10000, MaxMessageCount};
        }

        void SetUp() override
        {
            testLogPath = std::filesystem::temp_directory_path() / "nc_test.log";
            auto file = std::ofstream{testLogPath};
            if (!file)
            {
                throw nc::NcError{"Failed to create temporary log file"};
            }
        }

        void TearDown() override
        {
            if (!testLogPath.empty())
            {
                std::filesystem::remove(testLogPath);
            }
        }
};

TEST_F(LogTest, DefaultCallback_logsToStdout)
{
    constexpr auto subsystem = std::string_view{"Testing"};
    constexpr auto file = std::string_view{"Log_tests.cpp"};
    constexpr auto line = 100;
    constexpr auto info = std::string_view{"something informative, perhaps"};
    constexpr auto warning = std::string_view{"an undesirable"};
    constexpr auto error = std::string_view{"even worse"};
    constexpr auto trace = std::string_view{"chatty cathy"};

    {
        testing::internal::CaptureStdout();
        NC_LOG_INFO_EXT(subsystem, file, line, info);
        const auto expected = NC_LOG_FMT_MSG(nc::LogCategory::Info, subsystem, file, line, info);
        EXPECT_EQ(expected, testing::internal::GetCapturedStdout());
    }

    {
        testing::internal::CaptureStdout();
        NC_LOG_WARNING_EXT(subsystem, file, line, warning);
        const auto expected = NC_LOG_FMT_MSG(nc::LogCategory::Warning, subsystem, file, line, warning);
        EXPECT_EQ(expected, testing::internal::GetCapturedStdout());
    }

    {
        testing::internal::CaptureStdout();
        NC_LOG_ERROR_EXT(subsystem, file, line, error);
        const auto expected = NC_LOG_FMT_MSG(nc::LogCategory::Error, subsystem, file, line, error);
        EXPECT_EQ(expected, testing::internal::GetCapturedStdout());
    }

    {
        testing::internal::CaptureStdout();
        NC_LOG_TRACE_EXT(subsystem, file, line, trace);
        const auto expected = NC_LOG_FMT_MSG(nc::LogCategory::Verbose, subsystem, file, line, trace);
        EXPECT_EQ(expected, testing::internal::GetCapturedStdout());
    }
}

TEST_F(LogTest, SetLogCallback_registersFunction)
{
    static auto called = false;
    auto callback = [](nc::LogCategory category,
                       std::string_view subsystem,
                       std::string_view function,
                       int line,
                       std::string_view message)
    {
        called = true;
        EXPECT_EQ(nc::LogCategory::Info, category);
        EXPECT_STREQ("TestSubsystem", subsystem.data());
        EXPECT_STREQ("TestFile", function.data());
        EXPECT_EQ(42, line);
        EXPECT_STREQ("TestMessage", message.data());
    };

    nc::SetLogCallback(callback);
    NC_LOG_INFO_EXT("TestSubsystem", "TestFile", 42, "TestMessage");
    EXPECT_TRUE(called);
}

TEST_F(FileLoggerTest, Constructor_registersCallback)
{
    nc::SetLogCallback(nullptr);
    EXPECT_EQ(nullptr, nc::detail::LogCallback);
    auto uut = MakeUut();
    EXPECT_NE(nullptr, nc::detail::LogCallback);
    EXPECT_NO_THROW(NC_LOG_INFO("a message"));
}

TEST_F(FileLoggerTest, Destructor_flushes)
{
    EXPECT_EQ(0, std::filesystem::file_size(testLogPath));

    {
        auto uut = MakeUut();
        NC_LOG_INFO("a message");
        EXPECT_EQ(0, std::filesystem::file_size(testLogPath));
    }

    EXPECT_LT(0, std::filesystem::file_size(testLogPath));
}

TEST_F(FileLoggerTest, Destructor_resetsCallback)
{
    {
        auto uut = MakeUut();
    }

    // Logger should have reset to default logging to avoid leaving a dangling ptr registered
    EXPECT_EQ(nc::detail::DefaultLogCallback, nc::detail::LogCallback);
    testing::internal::CaptureStdout();
    NC_LOG_INFO("headed to stdout");
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(FileLoggerTest, Destructor_alternativeCallbackSet_doesNotResetCallback)
{
    static auto called = false;
    auto callback = [](nc::LogCategory,
                       std::string_view,
                       std::string_view,
                       int,
                       std::string_view)
    {
        called = true;
    };

    {
        auto uut = MakeUut();
        nc::SetLogCallback(callback);
        EXPECT_EQ(nc::detail::LogCallback, callback);
    }

    // Callback was changed out from under us - no dangling ptr left behind, so we should have left the callback as is.
    EXPECT_EQ(nc::detail::LogCallback, callback);
    EXPECT_NO_THROW(NC_LOG_INFO("test"));
    EXPECT_TRUE(called);
}

TEST_F(FileLoggerTest, Log_exceedSpecifiedMessageCount_flushes)
{
    static_assert(MaxMessageCount == 3ull);
    auto uut = MakeUut(); // logs an initial timestamp message
    NC_LOG_INFO("second message");
    NC_LOG_INFO("third message");
    EXPECT_EQ(0, std::filesystem::file_size(testLogPath));
    NC_LOG_INFO("should flush to file");
    EXPECT_LT(0, std::filesystem::file_size(testLogPath));
}
