/**
 * @file TaskFwd.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <cstdint>

namespace nc::task
{
template<class Phase>
class TaskGraph;

struct UpdatePhase;
struct RenderPhase;
using UpdateTasks = TaskGraph<UpdatePhase>;
using RenderTasks = TaskGraph<RenderPhase>;
} // namespace nc::task
