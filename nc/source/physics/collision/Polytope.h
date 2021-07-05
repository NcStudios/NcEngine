#pragma once

#include "Simplex.h"
#include "Manifold.h"

#include <vector>

namespace nc::physics
{
    class Polytope
    {
        public:
            void Initialize(const Simplex& simplex);
            auto GetNormalData(size_t index) const -> NormalData;
            auto ComputeNormalData() -> size_t;
            auto Expand(const Vector3& supportCSO, const Vector3& worldSupportA, const Vector3& worldSupportB, const Vector3& localSupportA, const Vector3& localSupportB, size_t* oldMinFace) -> bool;
            bool GetContacts(size_t minFace, Vector3* worldContactA, Vector3* worldContactB, Vector3* localContactA, Vector3* localContactB) const;

        private:
            std::vector<Vector3> m_vertices;
            std::vector<std::pair<Vector3, Vector3>> m_worldSupports;
            std::vector<std::pair<Vector3, Vector3>> m_localSupports;
            std::vector<size_t> m_indices;
            std::vector<NormalData> m_normals;
            std::vector<std::pair<size_t, size_t>> m_edges;
            std::vector<size_t> m_newIndices;
            std::vector<NormalData> m_newNormals;

            enum class DataSet { Merged, Unmerged };

            std::vector<size_t> InitialFaces();
            void CreateIndicesFromEdges();
            void MergeNewFeatures();
            void CheckEdge(size_t a, size_t b);
            void RemoveFace(size_t normalIndex);

            template<DataSet Set>
            auto GetIndices() const -> const std::vector<size_t>&;

            template<DataSet Set>
            auto GetNormals() -> std::vector<NormalData>&;

            template<DataSet Set>
            auto ComputeNormalData_() -> size_t;
    };

    template<Polytope::DataSet Set>
    auto Polytope::GetIndices() const -> const std::vector<size_t>&
    {
        if constexpr(Set == DataSet::Merged)
            return m_indices;
        else if(Set == DataSet::Unmerged)
            return m_newIndices;
    }

    template<Polytope::DataSet Set>
    auto Polytope::GetNormals() -> std::vector<NormalData>&
    {
        if constexpr(Set == DataSet::Merged)
            return m_normals;
        else if(Set == DataSet::Unmerged)
            return m_newNormals;
    }

    template<Polytope::DataSet Set>
    auto Polytope::ComputeNormalData_() -> size_t
    {
        size_t minTriangle = 0u;
        float minDistance = std::numeric_limits<float>::max();
        const auto& indices = GetIndices<Set>();
        auto& normals = GetNormals<Set>();

        for(size_t i = 0u; i < indices.size(); i += 3)
        {
            auto a = m_vertices[indices[i]];
            auto b = m_vertices[indices[i+1]];
            auto c = m_vertices[indices[i+2]];

            auto normal = Normalize(CrossProduct(b - a, c - a));
            auto distance = Dot(normal, a);

            if(distance < 0.0f)
            {
                normal = -normal;
                distance *= -1.0f;
            }

            normals.emplace_back(normal, distance);

            if(distance < minDistance)
            {
                minDistance = distance;
                minTriangle = i / 3;
            }
        }

        return minTriangle;
    }

} // namespace nc::physics