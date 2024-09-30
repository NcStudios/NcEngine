/**
 * @file HitInfo.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Entity.h"

#include <array>
#include <span>

namespace nc
{
/** @brief A list of contact points on a RigidBody. */
class Contacts
{
    public:
        explicit Contacts(const std::array<Vector3, 4>& points, size_t numPoints = 4)
            : m_points{points},
              m_numPoints{static_cast<uint32_t>(numPoints)}
        {
        }

        auto GetPoints() const -> std::span<const Vector3>
        {
            return std::span<const Vector3>{m_points.begin(), m_numPoints};
        }

        auto GetCount() const -> uint32_t
        {
            return m_numPoints;
        }

    private:
        std::array<Vector3, 4> m_points;
        uint32_t m_numPoints;
};

/** @brief Collision event information. */
struct HitInfo
{
    Vector3 offset = Vector3::Zero();
    Vector3 normal = Vector3::Zero();
    float depth = 0.0f;
    Contacts contactsOnFirst;
    Contacts contactsOnSecond;
};
} // namespace nc
