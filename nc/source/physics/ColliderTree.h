#pragma once

#include "Ecs.h"

#include <vector>


#include <iostream>

namespace
{
    nc::Collider::BoundingVolume CalculateBoundingVolume(nc::ColliderInfo info, const DirectX::XMMATRIX* transform)
    {
        switch(info.type)
        {
            case nc::ColliderType::Box:
            {
                DirectX::BoundingOrientedBox out({info.offset.x, info.offset.y, info.offset.z}, {info.scale.x / 2.0f, info.scale.y / 2.0f, info.scale.z / 2.0f}, {0,0,0,1.0f});
                out.Transform(out, *transform);
                return {out};
            }
            case nc::ColliderType::Sphere:
            {
                DirectX::BoundingSphere out{{info.offset.x, info.offset.y, info.offset.z}, info.scale.x / 2.0f};
                out.Transform(out, *transform);
                return {out};
            }
        }

        throw std::runtime_error("CreateBoundingVolume - Unknown ColliderType");
    }
}

namespace nc::physics
{
    constexpr uint32_t BranchDimension = 8u;
    constexpr uint32_t MaxVolumesPerSubspace = 10u;

    struct Subspace
    {
        struct HandleVolumePair
        {
            EntityHandle handle;
            Collider::BoundingVolume volume;
        };

        Subspace(DirectX::XMFLOAT3 center, float halfSideLength)
            : space{center, {halfSideLength, halfSideLength, halfSideLength}},
              volumes{},
              children{},
              isLeaf{true}
        {
            volumes.reserve(MaxVolumesPerSubspace);
            children.reserve(BranchDimension);
        }

        void Add(EntityHandle handle, const Collider::BoundingVolume& volume)
        {
            if(Contains(volume))
            {
                if(isLeaf)
                {
                    if(volumes.size() < MaxVolumesPerSubspace)
                    {
                        volumes.push_back({handle, volume});
                        return;
                    }

                    Branch();
                }
                
                AddToChildren(handle, volume);
            }
        }

        bool Contains(const Collider::BoundingVolume& volume)
        {
            return std::visit([this](auto&& a) { return a.Intersects(space); }, volume);
        }

        void Branch()
        {
            isLeaf = false;
            auto newHalfSideLength = space.Extents.x / 2.0f;
            auto& [centerX, centerY, centerZ] = space.Center;
            auto xMin = centerX - newHalfSideLength;
            auto xMax = centerX + newHalfSideLength;
            auto yMin = centerY - newHalfSideLength;
            auto yMax = centerY + newHalfSideLength;
            auto zMin = centerZ - newHalfSideLength;
            auto zMax = centerZ + newHalfSideLength;
            children.push_back({ {xMin, yMin, zMin}, newHalfSideLength });
            children.push_back({ {xMin, yMin, zMax}, newHalfSideLength });
            children.push_back({ {xMin, yMax, zMin}, newHalfSideLength });
            children.push_back({ {xMin, yMax, zMax}, newHalfSideLength });
            children.push_back({ {xMax, yMin, zMin}, newHalfSideLength });
            children.push_back({ {xMax, yMin, zMax}, newHalfSideLength });
            children.push_back({ {xMax, yMax, zMin}, newHalfSideLength });
            children.push_back({ {xMax, yMax, zMax}, newHalfSideLength });

            for(auto& [handle, volume] : volumes)
                AddToChildren(handle, volume);

            volumes.clear();
        }

        void AddToChildren(EntityHandle handle, const Collider::BoundingVolume& volume)
        {
            for(auto& space : children)
            {
                space.Add(handle, volume);
            }
        }

        void BroadCheck(const DirectX::BoundingSphere& dynamicVolume, std::vector<Subspace::HandleVolumePair*>* out)
        {
            if(isLeaf)
            {
                for(auto& pair : volumes)
                {
                    if(std::visit([&dynamicVolume](auto&& staticVolume) { return dynamicVolume.Intersects(staticVolume); }, pair.volume))
                        out->emplace_back(&pair);
                }

                return;
            }

            if(Contains(dynamicVolume))
            {
                for(auto& space : children)
                {
                    space.BroadCheck(dynamicVolume, out);
                }
            }
        }

        DirectX::BoundingBox space;
        std::vector<HandleVolumePair> volumes;
        std::vector<Subspace> children;
        bool isLeaf;
    };

    class ColliderTree
    {
        constexpr static auto initialExtents = 500.0f;

        public:
            ColliderTree()
                : m_root{{0.0f, 0.0f, 0.0f}, initialExtents}
            {
            }

            void Add(EntityHandle handle, const ColliderInfo& info)
            {
                auto volume = CalculateBoundingVolume(info, &GetComponent<Transform>(handle)->GetTransformationMatrix());
                m_root.Add(handle, volume);
            }

            void Remove(EntityHandle handle)
            {
                (void)handle;
            }

            void Clear()
            {
                m_root.volumes.clear();
                m_root.children.clear();
                m_root.isLeaf = true;
            }

            std::vector<Subspace::HandleVolumePair*> BroadCheck(const DirectX::BoundingSphere& volume)
            {
                std::vector<Subspace::HandleVolumePair*> out;
                m_root.BroadCheck(volume, &out);
                return out;
            }

        private:
            Subspace m_root;
    };

} // namespace nc::physics