#include "BspTree.h"
#include "assets/AssetService.h"

#include <iostream>

namespace
{
using namespace nc;
using namespace nc::physics;

auto CreateTriMesh(Registry* registry, const ConcaveCollider& collider) -> TriMesh
{
    auto meshView = AssetService<ConcaveColliderView>::Get()->Acquire(collider.GetPath());
    auto entity = collider.ParentEntity();
    auto* transform = registry->Get<Transform>(entity);
    const auto& m = transform->TransformationMatrix();
    const auto& scale = transform->Scale();
    auto maxScale = Max(Max(scale.x, scale.y), scale.z);
    auto estimate = Sphere{transform->Position(), maxScale * meshView.maxExtent};

    std::vector<Triangle> triangles;
    triangles.reserve(meshView.triangles.size());
    Vector3 a, b, c;

    for(const auto& triangle : meshView.triangles)
    {
        /** @todo could just store the xmvecs instead of loading. More of
         *  and issure when checking collisions. */
        auto a_v = DirectX::XMLoadVector3(&triangle.a);
        auto b_v = DirectX::XMLoadVector3(&triangle.b);
        auto c_v = DirectX::XMLoadVector3(&triangle.c);
        a_v = DirectX::XMVector3Transform(a_v, m);
        b_v = DirectX::XMVector3Transform(b_v, m);
        c_v = DirectX::XMVector3Transform(c_v, m);
        DirectX::XMStoreVector3(&a, a_v);
        DirectX::XMStoreVector3(&b, b_v);
        DirectX::XMStoreVector3(&c, c_v);
        triangles.emplace_back(a, b, c);
    }

    return TriMesh
    {
        .triangles = std::move(triangles),
        .estimate = estimate,
        .entity = entity
    };
}

auto KeepMaximum(Vector3 vec) -> Vector3
{
    if(vec.x >= vec.y)
    {
        vec.y = 0.0f;
        if(vec.x >= vec.z) vec.z = 0.0f;
        else vec.x = 0.0f;
    }
    else
    {
        vec.x = 0.0f;
        if(vec.y >= vec.z) vec.z = 0.0f;
        else vec.y = 0.0f;
    }

    return vec;
}
} // anonymous namespace

namespace nc::physics
{
BspTree::BspTree(Registry* registry, Signal<>& rebuildStatics)
    : m_registry{registry},
      m_nodes{},
      m_triMeshes{},
      m_results{},
      m_onAddConnection{registry->OnAdd<ConcaveCollider>().Connect(this, &BspTree::OnAdd)},
      m_onRemoveConnection{registry->OnRemove<ConcaveCollider>().Connect(this, &BspTree::OnRemove)},
      m_onRebuildStaticsConnection{rebuildStatics.Connect(this, &BspTree::Rebuild)}
{
    m_nodes.push_back(LeafNode{});
}

void BspTree::OnAdd(ConcaveCollider& collider)
{
    size_t index = m_triMeshes.size();
    m_triMeshes.push_back(CreateTriMesh(m_registry, collider));
    AddToTree(m_triMeshes.back(), index, 0u);
}

void BspTree::OnRemove(Entity entity)
{
    auto pos = std::ranges::find_if(m_triMeshes, [entity](auto&& triMesh)
    {
        return triMesh.entity == entity;
    });

    if(pos == m_triMeshes.end())
        throw NcError("Tree does not have a collider for entity");

    /** @todo We don't actually remove anything because it would mess
     *  up stored indices. We could use a free list, but removing static
     *  entities should not be done frequently. Worth it or not? */
    pos->triangles.clear();
    pos->entity = Entity::Null();

    const size_t meshIndex = pos - m_triMeshes.begin();

    for(auto& node : m_nodes)
    {
        if(auto* leaf = std::get_if<LeafNode>(&node); leaf)
            std::erase(leaf->triMeshIndices, meshIndex);
    }
}

void BspTree::Rebuild()
{
    m_triMeshes.clear();
    m_nodes.clear();
    m_nodes.push_back(LeafNode{});
    auto colliders = m_registry->StorageFor<ConcaveCollider>()->GetComponents();
    for (auto& collider : colliders)
    {
        OnAdd(collider);
    }
}

void BspTree::Clear()
{
    m_nodes.clear();
    m_nodes.push_back(LeafNode{});
    m_triMeshes.clear();
    m_results.contacts.clear();
    m_results.contacts.shrink_to_fit();
    m_results.events.clear();
    m_results.events.shrink_to_fit();
}

void BspTree::AddToTree(const TriMesh& mesh, size_t meshIndex, size_t currentNodeIndex)
{
    auto& node = m_nodes.at(currentNodeIndex);

    if(auto* inner = std::get_if<InnerNode>(&node); inner)
    {
        AddToInnerNode(mesh, meshIndex, inner);
    }
    else
    {
        AddToLeafNode(mesh, meshIndex, currentNodeIndex);
    }
}

void BspTree::AddToInnerNode(const TriMesh& mesh, size_t meshIndex, InnerNode* innerNode)
{
    switch(TestHalfspace(innerNode->dividingPlane, mesh.estimate))
    {
        case HalfspaceContainment::Intersecting:
        {
            AddToTree(mesh, meshIndex, innerNode->positiveHalfspaceIndex);
            AddToTree(mesh, meshIndex, innerNode->negativeHalfspaceIndex);
            break;
        }
        case HalfspaceContainment::Positive:
        {
            AddToTree(mesh, meshIndex, innerNode->positiveHalfspaceIndex);
            break;
        }
        case HalfspaceContainment::Negative:
        {
            AddToTree(mesh, meshIndex, innerNode->negativeHalfspaceIndex);
            break;
        }
    }
}

void BspTree::AddToLeafNode(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex)
{
    auto& leafNode = std::get<LeafNode>(m_nodes.at(leafNodeIndex));
    if(leafNode.triMeshIndices.size() < NodeCapacity || m_nodes.size() > MaxNodeCount)
    {
        leafNode.triMeshIndices.push_back(meshIndex);
        return;
    }

    SplitLeaf(mesh, meshIndex, leafNodeIndex, &leafNode);
}

void BspTree::SplitLeaf(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex, LeafNode* leaf)
{
    auto plane = FindSplitPlane(*leaf);

    /** Add existing indices to new positive/negative children nodes. */
    LeafNode positiveNode, negativeNode;
    for(auto i : leaf->triMeshIndices)
    {
        switch(TestHalfspace(plane, m_triMeshes.at(i).estimate))
        {
            case HalfspaceContainment::Intersecting:
            {
                positiveNode.triMeshIndices.push_back(i);
                negativeNode.triMeshIndices.push_back(i);
                break;
            }
            case HalfspaceContainment::Positive:
            {
                positiveNode.triMeshIndices.push_back(i);
                break;
            }
            case HalfspaceContainment::Negative:
            {
                negativeNode.triMeshIndices.push_back(i);
                break;
            }
        }
    }

    /** If resulting nodes would require a split, skip and allow exceeding capacity. */
    const size_t positiveCount = positiveNode.triMeshIndices.size();
    const size_t negativeCount = negativeNode.triMeshIndices.size();
    if(positiveCount > NodeCapacity || negativeCount > NodeCapacity)
    {
        /** @todo Remove print statement once values are tuned. */
        std::cout << "BspTree - Skipping partition due to too many intersections\n";
        leaf->triMeshIndices.push_back(meshIndex);
        return;
    }

    /** Add new nodes to container and update current leaf node to an inner node. */
    size_t positiveNodeIndex = m_nodes.size();
    size_t negativeNodeIndex = positiveNodeIndex + 1;
    m_nodes.emplace_back(std::move(positiveNode));
    m_nodes.emplace_back(std::move(negativeNode));
    m_nodes.at(leafNodeIndex) = node_type{InnerNode{plane, positiveNodeIndex, negativeNodeIndex}};

    /** Add pending mesh to either child node. */
    switch(TestHalfspace(plane, mesh.estimate))
    {
        case HalfspaceContainment::Intersecting:
        {
            AddToLeafNode(mesh, meshIndex, positiveNodeIndex);
            AddToLeafNode(mesh, meshIndex, negativeNodeIndex);
            break;
        }
        case HalfspaceContainment::Positive:
        {
            AddToLeafNode(mesh, meshIndex, positiveNodeIndex);
            break;
        }
        case HalfspaceContainment::Negative:
        {
            AddToLeafNode(mesh, meshIndex, negativeNodeIndex);
            break;
        }
    }
}

auto BspTree::FindSplitPlane(const LeafNode& node) -> Plane
{
    /** Plane selection strategy:
     *  Compute the average and variance of the contained sphere centers.
     * 
     *  Find the axis for which the variance is maximized to use as the plane normal.
     * 
     *  The plane will lie tangent to the sphere closest to the average center. Given
     *  the selected normal, this leaves two possible positions. Find the one closer
     *  to the average.
     * 
     *  Nudge the point to avoid an intersection between the sphere and plane.
     * 
     *  Find the shortest signed distance from the origin to the plane. */

    auto [averageCenter, normal, closestMeshIndex] = ComputePartitionData(node.triMeshIndices);
    const auto& closestMesh = m_triMeshes.at(closestMeshIndex);
    auto closestCenterToAverageCenter = averageCenter - closestMesh.estimate.center;
    auto offsetDirection = (Dot(closestCenterToAverageCenter, Vector3{1,1,1}) >= 0.0f) ? 1.0f : -1.0f;
    auto offset = normal * (closestMesh.estimate.radius + PlaneEpsilon) * offsetDirection;
    auto originToPlane = HadamardProduct(normal, closestMesh.estimate.center + offset);
    auto distanceDirection = (Dot(originToPlane, normal) >= 0.0f) ? 1.0f : -1.0f;
    auto distance = distanceDirection * Magnitude(originToPlane);

    return Plane{normal, distance};
}

auto BspTree::ComputePartitionData(const std::vector<size_t>& meshIndices) -> PartitionData
{
    const auto indexCount = meshIndices.size();

    if(indexCount == 0u)
        throw NcError("Empty vector");

    auto averageCenter = Vector3::Zero();

    for(auto meshIndex : meshIndices)
    {
        averageCenter += m_triMeshes.at(meshIndex).estimate.center;
    }

    averageCenter /= static_cast<float>(indexCount);
    auto variance = Vector3::Zero();
    float minSquareDistance = std::numeric_limits<float>::max();
    size_t closestIndex = 0u; // only safe if we throw on empty vector

    for(auto meshIndex : meshIndices)
    {
        const auto& mesh = m_triMeshes.at(meshIndex);
        auto dif = mesh.estimate.center - averageCenter;
        variance += HadamardProduct(dif, dif);
        float squareDistance = SquareDistance(mesh.estimate.center, averageCenter);
        if(squareDistance < minSquareDistance)
        {
            minSquareDistance = squareDistance;
            closestIndex = meshIndex;
        }
    }

    variance = KeepMaximum(variance / static_cast<float>(indexCount));
    return PartitionData{averageCenter, Normalize(variance), closestIndex};
}

void BspTree::BroadTest(size_t currentNodeIndex, const Sphere& estimate, std::vector<size_t>& narrowTestMeshIndices)
{
    auto& node = m_nodes.at(currentNodeIndex);

    if(auto* leaf = std::get_if<LeafNode>(&node); leaf)
    {
        narrowTestMeshIndices.insert(narrowTestMeshIndices.end(), leaf->triMeshIndices.begin(), leaf->triMeshIndices.end());
        return;
    }

    auto& inner = std::get<InnerNode>(node);
    switch(TestHalfspace(inner.dividingPlane, estimate))
    {
        case HalfspaceContainment::Intersecting:
        {
            BroadTest(inner.positiveHalfspaceIndex, estimate, narrowTestMeshIndices);
            BroadTest(inner.negativeHalfspaceIndex, estimate, narrowTestMeshIndices);
            break;
        }
        case HalfspaceContainment::Positive:
        {
            BroadTest(inner.positiveHalfspaceIndex, estimate, narrowTestMeshIndices);
            break;
        }
        case HalfspaceContainment::Negative:
        {
            BroadTest(inner.negativeHalfspaceIndex, estimate, narrowTestMeshIndices);
            break;
        }
    }
}
} // namespace nc::physics
