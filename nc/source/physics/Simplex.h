#pragma once

#include "math/Vector.h"

#include <array>

namespace nc::physics
{
    class Simplex
    {
        public:
            Simplex() : m_points(), m_size{0} {}

            template<size_t V1>
            void ToPoint();

            template<size_t V1, size_t V2>
            void ToLine();

            template<size_t V1, size_t V2, size_t V3>
            void ToTriangle();

            size_t size() const { return m_size; }
            auto begin() const { return m_points.begin(); }
            auto end() const { return m_points.end(); }
    
            Vector3& operator[](size_t i) { return m_points[i]; }

            void push_front(const Vector3& vertex)
            {
                m_points = { vertex, m_points[0], m_points[1], m_points[2] };
                m_size = std::min(m_size + 1u, size_t{4u});
            }

        private:
            std::array<Vector3, 4u> m_points;
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
    }

    template<>
    inline void Simplex::ToTriangle<0u, 2u, 3u>()
    {
        m_size = 3u;
        m_points[1] = m_points[2];
        m_points[2] = m_points[3];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 3u, 1u>()
    {
        m_size = 3u;
        m_points[2] = m_points[1];
        m_points[1] = m_points[3];
    }

    template<>
    inline void Simplex::ToTriangle<0u, 1u, 3u>()
    {
        m_points[2] = m_points[3];
        m_size = 3u;
    }
} // nc::physics