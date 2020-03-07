#pragma once
#include <vector>
#include "DirectXMath.h"

namespace nc::graphics::d3dresource
{

    template<class T>
    class IndexedTriangleList
    {
        public:
            IndexedTriangleList() = default;
            IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in)
                : vertices(std::move(verts_in)), indices(std::move(indices_in))
            {
                assert(vertices.size() > 2);
                assert(indices.size() % 3 == 0);
            }

            void Transform(DirectX::FXMMATRIX matrix)
            {
                for(auto& v : vertices)
                {
                    const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
                    DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, matrix));
                }
            }

            void SetNormalsIndependentFlat() noexcept
            {
                assert(indices.size() % 3 == 0 && indices.size() > 0);

                for(size_t i = 0; i < indices.size(); i += 3)
                {
                    auto& v0 = vertices[indices[i]];
                    auto& v1 = vertices[indices[i + 1]];
                    auto& v2 = vertices[indices[i + 2]];
                    const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
                    const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
                    const auto p2 = DirectX::XMLoadFloat3(&v2.pos);
                    const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross( (p1 - p0), (p2 - p0) ) );
                
                    DirectX::XMStoreFloat3(&v0.n, n);
                    DirectX::XMStoreFloat3(&v1.n, n);
                    DirectX::XMStoreFloat3(&v2.n, n);
                }
            }

        public:
            std::vector<T> vertices;
            std::vector<unsigned short> indices;
    };

}