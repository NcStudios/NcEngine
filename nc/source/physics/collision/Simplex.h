#pragma once

#include "math/Vector.h"

#include <array>

namespace nc::physics
{
    class Simplex
    {
        public:
            Simplex()
                : m_points(),
                  m_worldSupports(),
                  m_localSupports(),
                  m_size{0}
            {}

            template<size_t V1>
            void ToPoint();

            template<size_t V1, size_t V2>
            void ToLine();

            template<size_t V1, size_t V2, size_t V3>
            void ToTriangle();

            size_t size() const { return m_size; }
            auto PointsBegin() const { return m_points.begin(); }
            auto PointsEnd() const { return m_points.end(); }
    
            auto WorldSupportsBegin() const { return m_worldSupports.begin(); }
            auto WorldSupportsEnd() const { return m_worldSupports.end(); }

            auto LocalSupportsBegin() const { return m_localSupports.begin(); }
            auto LocalSupportsEnd() const { return m_localSupports.end(); }

            Vector3& operator[](size_t i) { return m_points[i]; }

            void push_front(const Vector3& vertex, const Vector3& worldSupportA, const Vector3& worldSupportB, const Vector3& localSupportA, const Vector3& localSupportB)
            {
                m_points = { vertex, m_points[0], m_points[1], m_points[2] };
                m_worldSupports = { std::pair{worldSupportA, worldSupportB}, m_worldSupports[0], m_worldSupports[1], m_worldSupports[2] };
                m_localSupports = { std::pair{localSupportA, localSupportB}, m_localSupports[0], m_localSupports[1], m_localSupports[2] };
                m_size = std::min(m_size + 1u, size_t{4u});
            }

        private:
            std::array<Vector3, 4u> m_points;
            std::array<std::pair<Vector3, Vector3>, 4u> m_worldSupports;
            std::array<std::pair<Vector3, Vector3>, 4u> m_localSupports;
            size_t m_size;
    };

    template<>
    inline void Simplex::ToPoint<0u>()
    {
        m_size = 1u;
    }

    template<>
    inline void Simplex::ToPoint<1u>()
    {
        m_size = 1u;
        m_points[0] = m_points[1];

        m_worldSupports[0] = m_worldSupports[1];
        m_localSupports[0] = m_localSupports[1];
    }

    template<>
    inline void Simplex::ToLine<0u, 1u>()
    {
        m_size = 2u;
    }

    template<>
    inline void Simplex::ToLine<0u, 2u>()
    {
        m_size = 2u;
        m_points[1] = m_points[2];

        m_worldSupports[1] = m_worldSupports[2];
        m_localSupports[1] = m_localSupports[2];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 1u, 2u>()
    {
        m_size = 3u;
    }

    template<>
    inline void Simplex::ToTriangle<0u, 2u, 1u>()
    {
        m_size = 3u;
        m_points[3] = m_points[1];
        m_points[1] = m_points[2];
        m_points[2] = m_points[3];

        m_worldSupports[3] = m_worldSupports[1];
        m_worldSupports[1] = m_worldSupports[2];
        m_worldSupports[2] = m_worldSupports[3];

        m_localSupports[3] = m_localSupports[1];
        m_localSupports[1] = m_localSupports[2];
        m_localSupports[2] = m_localSupports[3];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 2u, 3u>()
    {
        m_size = 3u;
        m_points[1] = m_points[2];
        m_points[2] = m_points[3];

        m_worldSupports[1] = m_worldSupports[2];
        m_worldSupports[2] = m_worldSupports[3];

        m_localSupports[1] = m_localSupports[2];
        m_localSupports[2] = m_localSupports[3];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 3u, 1u>()
    {
        m_size = 3u;
        m_points[2] = m_points[1];
        m_points[1] = m_points[3];

        m_worldSupports[2] = m_worldSupports[1];
        m_worldSupports[1] = m_worldSupports[3];
    
        m_localSupports[2] = m_localSupports[1];
        m_localSupports[1] = m_localSupports[3];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 1u, 3u>()
    {
        m_size = 3u;
        m_points[2] = m_points[3];

        m_worldSupports[2] = m_worldSupports[3];

        m_localSupports[2] = m_localSupports[3];
    }
} // nc::physics