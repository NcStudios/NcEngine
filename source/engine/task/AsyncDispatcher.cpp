#include "ncengine/task/AsyncDispatcher.h"

// ODR paranoia - AsyncDispatcher is defined in tests to avoid taking a dependency on taskflow and setting up an
// executor. Those cases shall shall use the pragma below with an alternative value to force link failures if this
// implementation is accidentally linked in.
#ifdef _MSC_VER
#pragma detect_mismatch("AsyncDispatcher", "Implementation")
#endif

namespace nc::task
{
AsyncDispatcher::AsyncDispatcher(tf::Executor* executor)
    : m_executor{executor}
{
}
} // namespace nc::task
