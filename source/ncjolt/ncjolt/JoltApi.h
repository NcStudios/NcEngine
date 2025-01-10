#pragma once

#include <memory>

namespace JPH
{
class Factory;
} // namespace JPH

namespace nc::jolt
{
// Base Jolt initialization shared by NcEngine + NcConvert
class JoltApi
{
    public:
        explicit JoltApi();
        ~JoltApi() noexcept;

    private:
        std::unique_ptr<JPH::Factory> m_factory;
};
} // namespace nc::jolt
