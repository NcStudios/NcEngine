#pragma once

#include "fmt/format.h"
#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
template<typename T>
void RotateElementToBeginning(std::vector<T>& vectorToRotate, const T& elem)
{
    auto elemPos = std::ranges::find(vectorToRotate, elem);
    NC_ASSERT(elemPos != vectorToRotate.end(), fmt::format("{0} was not present in the vector.", elem));
    T element = *elemPos;
    vectorToRotate.erase(elemPos);
    vectorToRotate.insert(vectorToRotate.begin(), element);
}
} // namespace nc::graphics