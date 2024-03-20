#pragma once

#define NC_PHYSICS_ASSERT_ENABLED 1      // Enable extra sanity checks
#define NC_PHYSICS_LOGGING 1             // Global debug logging control
#define NC_PHYSICS_COLLISION_LOGGING 0   // Enable extra collision/contact generation logging
#define NC_PHYSICS_CONTACT_LOGGING 0     // Enable extra contact persistence logging
#define NC_PHYSICS_SOLVER_LOGGING 0      // Enable extra collision resolution logging
#define NC_PHYSICS_DRAW_CONTACT_POINTS 0 // Enable wireframe rendering of contact points

#if NC_PHYSICS_ASSERT_ENABLED
#define NC_PHYSICS_ASSERT(expr, msg) NC_ASSERT(expr, msg)
#else
#define NC_PHYSICS_ASSERT(expr, msg)
#endif

#if NC_PHYSICS_LOGGING
#include <iostream>

inline void PhysicsDebugLog() {}

template<typename... Args>
void PhysicsDebugLog(const Args&... args)
{
    (std::cout << ... << args) << '\n';
}

#if NC_PHYSICS_COLLISION_LOGGING
#define NC_LOG_COLLISION(...) PhysicsDebugLog(__VA_ARGS__);
#else
#define NC_LOG_COLLISION(...)
#endif

#if NC_PHYSICS_CONTACT_LOGGING
#define NC_LOG_CONTACTS(...) PhysicsDebugLog(__VA_ARGS__);
#else
#define NC_LOG_CONTACTS(...)
#endif

#if NC_PHYSICS_SOLVER_LOGGING
#define NC_LOG_SOLVER(...) PhysicsDebugLog(__VA_ARGS__);
#else
#define NC_LOG_SOLVER(...)
#endif

#else
#define NC_LOG_COLLISION(...)
#define NC_LOG_CONTACTS(...)
#define NC_LOG_SOLVER(...)
#endif

#if NC_PHYSICS_DRAW_CONTACT_POINTS
#include "ncengine/debug/DebugRendering.h"
#include "ncengine/utility/MatrixUtilities.h"
#define NC_PHYSICS_DRAW_CONTACT(contact) \
NC_DEBUG_DRAW_WIREFRAME(nc::debug::WireframeType::Cube, nc::ComposeMatrix(nc::Vector3::Splat(0.05f), nc::Quaternion::Identity(), contact.worldPointA)); \
NC_DEBUG_DRAW_WIREFRAME(nc::debug::WireframeType::Cube, nc::ComposeMatrix(nc::Vector3::Splat(0.05f), nc::Quaternion::Identity(), contact.worldPointB));
#else
#define NC_PHYSICS_DRAW_CONTACT(contact);
#endif
