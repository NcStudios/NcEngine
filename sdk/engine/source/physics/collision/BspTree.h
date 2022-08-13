#pragma once

#include "ecs/Registry.h"
#include "physics/ConcaveCollider.h"
#include "physics/PhysicsPipelineTypes.h"
#include "physics/collision/IntersectionQueries.h"

#include <variant>

namespace nc::physics
{
/** Internal ConcaveCollider representation */
struct TriMesh
{
    std::vector<Triangle> triangles;
    Sphere estimate;
    Entity entity;
};

/** Binary space partitioning tree for testing collisions against mesh
 *  colliders. */
class BspTree
{
    /** Inner nodes are defined by a plane which creates two children -
     *  one for each halfspace. */
    struct InnerNode
    {
        Plane dividingPlane;
        size_t positiveHalfspaceIndex;
        size_t negativeHalfspaceIndex;
    };

    /** Leaf nodes contain the indices of contained meshes, allowing
     *  all meshes to be stored in a single vector. */
    struct LeafNode
    {
        std::vector<size_t> triMeshIndices;
    };

    /** Analysis of meshes in a leaf node used for splitting. */
    struct PartitionData
    {
        Vector3 averageCenter;
        Vector3 planeNormal;
        size_t closestMeshIndex;
    };

    using node_type = std::variant<InnerNode, LeafNode>;

    static constexpr size_t MaxNodeCount = 15u;
    static constexpr size_t NodeCapacity = 3u;
    static constexpr float PlaneEpsilon = 0.01f;

    public:
        BspTree(Registry* registry);

        void OnAdd(ConcaveCollider& collider);
        void OnRemove(Entity entity);

        template<Proxy ProxyType>
        void FindPairs(std::span<const ProxyType> proxies);
        auto Pairs() const -> const NarrowPhysicsResult& { return m_results; }
        void Clear();

    private:
        Registry* m_registry;
        std::vector<node_type> m_nodes;
        std::vector<TriMesh> m_triMeshes;
        NarrowPhysicsResult m_results;
        Connection<ConcaveCollider&> m_onAddConnection;
        Connection<Entity> m_onRemoveConnection;

        void AddToTree(const TriMesh& mesh, size_t meshIndex, size_t currentNodeIndex);
        void AddToInnerNode(const TriMesh& mesh, size_t meshIndex, InnerNode* innerNode);
        void AddToLeafNode(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex);
        void SplitLeaf(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex, LeafNode* leaf);
        auto FindSplitPlane(const LeafNode& node) -> Plane;
        auto ComputePartitionData(const std::vector<size_t>& meshIndices) -> PartitionData;
        void BroadTest(size_t currentNodeIndex, const Sphere& estimate, std::vector<size_t>& narrowTestMeshIndices);
};

template<Proxy ProxyType>
void BspTree::FindPairs(std::span<const ProxyType> proxies)
{
    m_results.contacts.clear();
    m_results.events.clear();

    if(m_nodes.empty())
        return;

    std::vector<size_t> narrowTestMeshIndices;
    narrowTestMeshIndices.reserve(NodeCapacity); // lower-bound guess
    CollisionState state;
    const size_t dynamicCount = proxies.size();

    for(size_t i = 0u; i < dynamicCount; ++i)
    {
        const auto& proxy = proxies[i];

        /** Find mesh colliders that are in the same region as the estimate. */
        narrowTestMeshIndices.clear();
        BroadTest(0u, proxy.estimate, narrowTestMeshIndices);

        for(auto meshIndex : narrowTestMeshIndices)
        {
            const auto& mesh = m_triMeshes[meshIndex];

            /** Broad check against the mesh estimate */
            if(!Intersect(proxy.estimate, mesh.estimate))
                continue;

            /** Narrow check against each mesh triangle until we find a collision. */
            for(const auto& triangle : mesh.triangles)
            {
                if(Collide(proxy.Volume(), triangle, proxy.Matrix(), &state))
                {
                    m_results.events.emplace_back(proxy.entity, mesh.entity, CollisionEventType::FirstBodyPhysics);
                    m_results.contacts.push_back(state.contact);
                    continue;
                }
            }
        }
    }
}
} // namespace nc::physics
