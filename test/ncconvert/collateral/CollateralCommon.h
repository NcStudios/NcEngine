#pragma once

#ifndef TEST_COLLATERAL_DIR
#error TEST_COLLATERAL_DIR must be defined for tests that use collateral
#endif

#include <filesystem>

namespace collateral
{
const auto collateralDirectory = std::filesystem::path{TEST_COLLATERAL_DIR};
}
