#include "Polytope.h"

#include <algorithm>

namespace nc::physics
{
    void Polytope::Initialize(const Simplex& simplex)
    {
        m_vertices = std::vector<Vector3>(simplex.begin(), simplex.end());
        m_indices = InitialFaces();
        m_normals.clear();
        m_edges.clear();
        m_newIndices.clear();
        m_newNormals.clear();
    }

    auto Polytope::GetNormalData(size_t index) const -> NormalData
    {
        return m_normals[index];
    }

    auto Polytope::ComputeNormalData() -> size_t
    {
        return ComputeNormalData_<DataSet::Merged>();
    }

    auto Polytope::Expand(const Vector3& support, size_t* oldMinFace) -> bool
    {
        m_edges.clear();
        m_newIndices.clear();
        m_newNormals.clear();

        for(size_t i = 0u; i < m_normals.size(); ++i)
        {
            if(Dot(m_normals[i].normal, support) > 0.0f)
            {
                RemoveFace(i);
                --i;
            }
        }

        if(m_edges.empty())
            return false;
        
        CreateIndicesFromEdges();

        m_vertices.push_back(support);

        auto newMinFace = ComputeNormalData_<DataSet::Unmerged>();
        float newMinDistance = std::numeric_limits<float>::max();

        for(size_t i = 0u; i < m_normals.size(); ++i)
        {
            if(m_normals[i].distance < newMinDistance)
            {
                newMinDistance = m_normals[i].distance;
                *oldMinFace = i;
            }
        }

        if(m_newNormals[newMinFace].distance < newMinDistance)
            *oldMinFace = newMinFace + m_normals.size();

        MergeNewFeatures();

        return true;
    }

    std::vector<size_t> Polytope::InitialFaces()
    {
        return std::vector<size_t>{0,1,2,  0,3,1,  0,2,3,  1,3,2};
    }

    void Polytope::CreateIndicesFromEdges()
    {
        for(auto [i1, i2] : m_edges)
        {
            m_newIndices.push_back(i1);
            m_newIndices.push_back(i2);
            m_newIndices.push_back(m_vertices.size());
        }
    }

    void Polytope::MergeNewFeatures()
    {
        m_indices.insert(m_indices.end(), m_newIndices.begin(), m_newIndices.end());
        m_normals.insert(m_normals.end(), m_newNormals.begin(), m_newNormals.end());
    }

    void Polytope::CheckEdge(size_t a, size_t b)
    {
        auto reverse = std::ranges::find(m_edges, std::make_pair(m_indices[b], m_indices[a]));

        if(reverse != m_edges.end())
            m_edges.erase(reverse);
        else
            m_edges.emplace_back(m_indices[a], m_indices[b]);
    }

    void Polytope::RemoveFace(size_t normalIndex)
    {
        size_t index1 = normalIndex * 3u;
        size_t index2 = index1 + 1u;
        size_t index3 = index2 + 1u;

        CheckEdge(index1, index2);
        CheckEdge(index2, index3);
        CheckEdge(index3, index1);
        m_indices[index3] = m_indices.back();
        m_indices.pop_back();
        m_indices[index2] = m_indices.back();
        m_indices.pop_back();
        m_indices[index1]= m_indices.back();
        m_indices.pop_back();
        m_normals[normalIndex] = m_normals.back();
        m_normals.pop_back();
    }
}