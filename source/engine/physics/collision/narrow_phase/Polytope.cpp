#include "Polytope.h"
#include "debug/NcError.h"

#include <algorithm>

namespace
{
const auto InitialFaces = std::vector<size_t>{0u,1u,2u,  0u,3u,1u,  0u,2u,3u,  1u,3u,2u};
}

namespace nc::physics
{
void Polytope::Initialize(const Simplex& simplex)
{
    m_vertices.clear();
    std::copy(simplex.m_points.begin(), simplex.m_points.end(), std::back_inserter(m_vertices));
    
    m_worldSupports.clear();
    std::copy(simplex.m_worldSupports.begin(), simplex.m_worldSupports.end(), std::back_inserter(m_worldSupports));

    m_localSupports.clear();
    std::copy(simplex.m_localSupports.begin(), simplex.m_localSupports.end(), std::back_inserter(m_localSupports));

    m_indices.clear();
    std::copy(InitialFaces.begin(), InitialFaces.end(), std::back_inserter(m_indices));

    m_normals.clear();
    m_edges.clear();
    m_newIndices.clear();
    m_newNormals.clear();
}

auto Polytope::GetNormalData(size_t index) const -> NormalData
{
    return m_normals.at(index);
}

auto Polytope::ComputeNormalData() -> size_t
{
    return ComputeNormalData_<DataSet::Merged>();
}

auto Polytope::Expand(const Vector3& supportCSO, const Contact& contact, size_t* oldMinFace) -> bool
{
    m_edges.clear();
    m_newIndices.clear();
    m_newNormals.clear();

    for(size_t i = 0u; i < m_normals.size(); ++i)
    {
        if(Dot(m_normals.at(i).normal, supportCSO) > 0.0f)
        {
            RemoveFace(i);
            --i;
        }
    }

    if(m_edges.empty())
    {
        /** @todo How do we end up with 0 edges? It is pretty rare - potentially
         *  just a first frame thing on certain scenes. */
        return false;
    }

    CreateIndicesFromEdges();

    m_vertices.push_back(supportCSO);
    m_worldSupports.emplace_back(contact.worldPointA, contact.worldPointB);
    m_localSupports.emplace_back(contact.localPointA, contact.localPointB);

    auto newMinFace = ComputeNormalData_<DataSet::Unmerged>();
    float newMinDistance = std::numeric_limits<float>::max();

    for(size_t i = 0u; i < m_normals.size(); ++i)
    {
        if(m_normals.at(i).distance < newMinDistance)
        {
            newMinDistance = m_normals.at(i).distance;
            *oldMinFace = i;
        }
    }

    if(m_newNormals[newMinFace].distance < newMinDistance)
        *oldMinFace = newMinFace + m_normals.size();

    MergeNewFeatures();
    return true;
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
    auto reverse = std::ranges::find(m_edges, std::make_pair(m_indices.at(b), m_indices.at(a)));

    if(reverse != m_edges.end())
        m_edges.erase(reverse);
    else
        m_edges.emplace_back(m_indices.at(a), m_indices.at(b));
}

void Polytope::RemoveFace(size_t normalIndex)
{
    size_t index1 = normalIndex * 3u;
    size_t index2 = index1 + 1u;
    size_t index3 = index2 + 1u;

    CheckEdge(index1, index2);
    CheckEdge(index2, index3);
    CheckEdge(index3, index1);
    m_indices.at(index3) = m_indices.back();
    m_indices.pop_back();
    m_indices.at(index2) = m_indices.back();
    m_indices.pop_back();
    m_indices.at(index1)= m_indices.back();
    m_indices.pop_back();
    m_normals.at(normalIndex) = m_normals.back();
    m_normals.pop_back();
}

Vector3 BarycentricProjection(const Vector3& point, const Vector3& a, const Vector3& b, const Vector3& c)
{
    auto v0 = b - a;
    auto v1 = c - a;
    auto v2 = point - a;
    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return Vector3{u, v, w};
}

bool Polytope::GetContacts(size_t minFace, Contact* contact) const
{
    size_t ii = minFace * 3u;
    size_t i1 = m_indices.at(ii);
    size_t i2 = m_indices.at(ii + 1u);
    size_t i3 = m_indices.at(ii + 2u);
    auto normal = Normalize(m_normals.at(minFace).normal);
    float distanceFromOrigin = m_normals.at(minFace).distance;
    const auto& cso1 = m_vertices.at(i1);
    const auto& cso2 = m_vertices.at(i2);
    const auto& cso3 = m_vertices.at(i3);
    auto [u, v, w] = BarycentricProjection(normal * distanceFromOrigin, cso1, cso2, cso3);

    /** @todo Still leaving this check. Checking for duplicate vertices in Gjk
     *  before moving to Epa hopefully fixed this. */
    if(std::isnan(u) || std::isnan(v) || std::isnan(w))
    {
        throw NcError("NaN in Barycentric Projection");
    }

    const auto& [worldA1, worldB1] = m_worldSupports.at(i1);
    const auto& [worldA2, worldB2] = m_worldSupports.at(i2);
    const auto& [worldA3, worldB3] = m_worldSupports.at(i3);

    contact->worldPointA = u * worldA1 + v * worldA2 + w * worldA3;
    contact->worldPointB = u * worldB1 + v * worldB2 + w * worldB3;

    const auto& [localA1, localB1] = m_localSupports.at(i1);
    const auto& [localA2, localB2] = m_localSupports.at(i2);
    const auto& [localA3, localB3] = m_localSupports.at(i3);

    contact->localPointA = u * localA1 + v * localA2 + w * localA3;
    contact->localPointB = u * localB1 + v * localB2 + w * localB3;
    
    return true;
}
} // namespace nc::physics
