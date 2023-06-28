#pragma once

namespace nc
{
namespace config
{
struct Config;
} // namespace config

namespace editor
{
auto MakeEngineConfig() -> config::Config;
} // namespace editor
} // namespace nc
