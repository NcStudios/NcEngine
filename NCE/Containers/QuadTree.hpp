#ifndef NCE_CONTAINERS_QUADTREE 
#define NCE_CONTAINERS_QUADTREE

#include <vector>
#include <memory>

#include "../Common/Vector.hpp"
#include "../Components/Collider.hpp"


#include <iostream>

namespace NCE::Containers
{
    class QuadTree
    {
        private:
            int _maxDensity;
            int _currentDensity = 0;
            NCE::Common::Vector4 _localQuad;
            std::vector<std::weak_ptr<NCE::Components::Collider>> _containedElements;

            bool _isPartitioned = false;
            
            std::unique_ptr<QuadTree> _childQuad1, _childQuad2, _childQuad3, _childQuad4;

            bool AreIntersecting(const NCE::Common::Vector4 &t_first, const NCE::Common::Vector4 &t_second);
            void Partition();

        public:
            QuadTree(int t_maxDensity, NCE::Common::Vector4 t_localQuad);
            
            void AddElement(std::weak_ptr<NCE::Components::Collider> t_newElement);
            
            void CheckCollisions();
            void Clear();
    };
}

#endif