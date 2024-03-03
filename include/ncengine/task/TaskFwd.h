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

enum class UpdatePhase : uint8_t;
enum class RenderPhase : uint8_t;
using UpdateTasks = TaskGraph<UpdatePhase>;
using RenderTasks = TaskGraph<RenderPhase>;
} // namespace nc::task
