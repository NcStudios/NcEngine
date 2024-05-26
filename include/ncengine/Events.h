#pragma once

#include "ncengine/type/StableAddress.h"
#include "ncengine/utility/Signal.h"

namespace nc
{
/** @brief Signals for internal engine events. */
struct SystemEvents : public StableAddress
{
    /**
     * @brief Notify NcEngine to quit after completion of the current frame.
     * @note NcEngine subscribes to this event with SignalPriority::Lowest. Subscribing
     *       with a higher priority allows being notified of the event prior to any state
     *       being cleared.
     */
    Signal<> quit;

    /**
     * @brief Event fired when static Entity data is made stale.
     * @note This operation may be expensive and is intended for editor and debug purposes.
     */
    Signal<> rebuildStatics;
};
} // namespace nc
