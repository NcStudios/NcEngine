#ifndef NCE_CONTAINERS_QUADTREE 
#define NCE_CONTAINERS_QUADTREE

#include <vector>
#include <memory>
#include <iostream>

#include "../Common/Vector.hpp"
#include "../Components/Collider.hpp"

namespace NCE::Containers
{
    class QuadTree
    {
        private:
            int m_currentDensity = 0;
            int m_maxDensity;
            bool m_isPartitioned = false;
            NCE::Common::Vector4 m_area;
            std::vector<std::weak_ptr<NCE::Components::Collider>> m_containedElements;
            std::unique_ptr<QuadTree> m_childQuad1, m_childQuad2, m_childQuad3, m_childQuad4;

            bool AreIntersecting(const NCE::Common::Vector4 &first_, const NCE::Common::Vector4 &second_);
            void Partition();

        public:
            QuadTree(int maxDensity_, NCE::Common::Vector4 area_);
            void AddElement(std::weak_ptr<NCE::Components::Collider> &newElement_);
            void CheckCollisions();
            void Clear();
    };
}

#endif