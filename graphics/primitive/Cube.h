#pragma once
#include "IndexedTriangleList.h"
#include "DirectXMath.h"

namespace nc::graphics::primitive
{
    class Cube
    {
        public:
            template<class VertType>
            static graphics::internal::IndexedTriangleList<VertType> Make()
            {
                constexpr float side = 1.0f / 2.0f;

                std::vector<DirectX::XMFLOAT3> vertices;
                vertices.emplace_back(-side, -side, -side ); // 0
                vertices.emplace_back( side, -side, -side ); // 1
                vertices.emplace_back(-side,  side, -side ); // 2
                vertices.emplace_back( side,  side, -side ); // 3
                vertices.emplace_back(-side, -side,  side ); // 4
                vertices.emplace_back( side, -side,  side ); // 5
                vertices.emplace_back(-side,  side,  side ); // 6
                vertices.emplace_back( side,  side,  side ); // 7

                std::vector<VertType> verts(vertices.size());

                for(size_t i = 0; i < vertices.size(); i++)
                {
                    verts[i].pos = vertices[i];
                }

                return
                {
                    std::move( verts ),
                    {
                        0,2,1, 2,3,1,
                        1,3,5, 3,7,5,
                        2,6,3, 3,6,7,
                        4,5,7, 4,7,6,
                        0,4,2, 2,4,6,
                        0,1,4, 1,5,4
                    }
                };
            }

            template<class VertType>
            static graphics::internal::IndexedTriangleList<VertType> MakeIndependent()
            {
                constexpr float side = 1.0 / 2.0f;
                std::vector<VertType> vertices(24);

                vertices[0].pos =  { -side, -side, -side };
                vertices[1].pos =  {  side, -side, -side };
                vertices[2].pos =  { -side,  side, -side };
                vertices[3].pos =  {  side,  side, -side };

                vertices[4].pos =  { -side, -side,  side };
                vertices[5].pos =  {  side, -side,  side };
                vertices[6].pos =  { -side,  side,  side };
                vertices[7].pos =  {  side,  side,  side };

                vertices[8].pos =  { -side, -side, -side };
                vertices[9].pos =  { -side,  side, -side };
                vertices[10].pos = { -side, -side,  side };
                vertices[11].pos = { -side,  side,  side };

                vertices[12].pos = {  side, -side, -side };
                vertices[13].pos = {  side,  side, -side };
                vertices[14].pos = {  side, -side,  side };
                vertices[15].pos = {  side,  side,  side };

                vertices[16].pos = { -side, -side, -side };
                vertices[17].pos = {  side, -side, -side };
                vertices[18].pos = { -side, -side,  side };
                vertices[19].pos = {  side, -side,  side };

                vertices[20].pos = { -side,  side, -side };
                vertices[21].pos = {  side,  side, -side };
                vertices[22].pos = { -side,  side,  side };
                vertices[23].pos = {  side,  side,  side };

                return
                {
                    std::move(vertices), 
                    {
                         0,  2,  1,   2,  3,  1,
                         4,  5,  7,   4,  7,  6,
                         8, 10,  9,  10, 11,  9,
                        12, 13, 15,  12, 15, 14,
                        16, 17, 18,  18, 17, 19,
                        20, 23, 21,  20, 22, 23
                    }
                };
            }
    };

} //end namespace nc::graphics::primitive