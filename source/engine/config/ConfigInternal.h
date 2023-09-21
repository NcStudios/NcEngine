#pragma once

namespace nc::config
{
struct Config;

void SetConfig(const Config& config);
auto Read(std::istream& stream) -> Config;
void Write(std::ostream& stream, const Config& in, bool writeSections = true);
}
