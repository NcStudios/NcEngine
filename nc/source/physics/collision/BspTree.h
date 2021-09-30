#pragma once

#include "Ecs.h"
#include "CollisionCache.h"

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
            BspTree(registry_type* registry);
            void OnAdd(ConcaveCollider& collider);
            void OnRemove(Entity entity);
            void Clear();
            void CheckCollisions(std::span<const DirectX::XMMATRIX> matrices,
                                 std::span<const ColliderEstimate> estimates,
                                 NarrowPhysicsResult* out);

        private:
            std::vector<node_type> m_nodes;
            std::vector<TriMesh> m_triMeshes;
            registry_type* m_registry;
            size_t m_previousContactCount;

            void AddToTree(const TriMesh& mesh, size_t meshIndex, size_t currentNodeIndex);
            void AddToInnerNode(const TriMesh& mesh, size_t meshIndex, InnerNode* innerNode);
            void AddToLeafNode(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex);
            void SplitLeaf(const TriMesh& mesh, size_t meshIndex, size_t leafNodeIndex, LeafNode* leaf);
            auto FindSplitPlane(const LeafNode& node) -> Plane;
            auto ComputePartitionData(const std::vector<size_t>& meshIndices) -> PartitionData;
            void BroadTest(size_t currentNodeIndex, const ColliderEstimate& estimate, std::vector<size_t>& narrowTestMeshIndices);
    };
}