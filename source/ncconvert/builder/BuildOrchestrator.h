#pragma once

#include "Config.h"

#include <memory>

namespace nc::convert
{
class Builder;

/** @brief Manager that handles dispatching instructions to the Builder. */
class BuildOrchestrator
{
    public:
        BuildOrchestrator(Config config);
        ~BuildOrchestrator() noexcept;

        /** @brief Build all required nca files. */
        void RunBuild();

    private:
        Config m_config;
        std::unique_ptr<Builder> m_builder;
};
} // namespace nc::convert
