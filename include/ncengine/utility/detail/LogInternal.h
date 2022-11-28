#pragma once

#include <string_view>

namespace nc::utility::detail
{
void InitializeLog(std::string_view path);
void CloseLog() noexcept;
void Log(std::string_view item, char prefix) noexcept;
void Log(const std::exception& e) noexcept;
}
