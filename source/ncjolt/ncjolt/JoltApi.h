#pragma once

#include <memory>

namespace JPH
{
class Factory;
} // namespace JPH

namespace nc::jolt
{
// Function type for handling Jolt asserts. Should return true to trigger breakpoint.
using AssertFailedCallback = bool(*)(const char* expression,
                                     const char* message,
                                     const char* file,
                                     unsigned line);

// Base Jolt initialization shared by NcEngine + NcConvert
class JoltApi
{
    public:
        explicit JoltApi(AssertFailedCallback assertCB = nullptr);
        ~JoltApi() noexcept;

    private:
        std::unique_ptr<JPH::Factory> m_factory;
};
} // namespace nc::jolt
