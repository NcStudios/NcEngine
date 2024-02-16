#pragma once

#include "ncengine/utility/Signal.h"

namespace nc
{
/** @brief Signals for internal engine events. */
struct SystemEvents
{
    /**
     * @brief Event fired when static Entity data is made stale.
     * @note This operation may be expensive and is intended for editor and debug purposes.
     */
    Signal<> rebuildStatics;
};
} // namespace nc
