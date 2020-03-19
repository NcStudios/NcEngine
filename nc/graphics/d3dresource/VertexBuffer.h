#pragma once
#include "GraphicsResource.h"
#include "DXException.h"

#include "Model.h" //for Vertex

#include <iostream>

namespace nc::graphics::d3dresource
{
    class VertexBuffer : public GraphicsResource
    {
        public:
            VertexBuffer(Graphics& graphics,const std::vector<Vertex>& vertices, const std::string& tag)
                : m_stride(sizeof(Vertex)), m_tag(tag)
            {
                D3D11_BUFFER_DESC bd      = {};
                bd.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
                bd.Usage                  = D3D11_USAGE_DEFAULT;
                bd.CPUAccessFlags         = 0u;
                bd.MiscFlags              = 0u;
                bd.ByteWidth              = UINT(m_stride * vertices.size());
                bd.StructureByteStride    = m_stride;

                D3D11_SUBRESOURCE_DATA sd = {};
                sd.pSysMem                = vertices.data();

                std::cout << "vertices.data(): " << sd.pSysMem << std::endl;

                ThrowIfFailed
                (
                    GetDevice(graphics)->CreateBuffer(&bd,&sd,&m_vertexBuffer),
                    __FILE__, __LINE__
                );
            }

            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics,
                                                             const std::vector<Vertex>& vertices,
                                                             const std::string& tag)
            {
                return GraphicsResourceManager::Aquire<VertexBuffer>(graphics, vertices, tag);
            }

            static std::string GetUID(const std::vector<Vertex>& vertices, const std::string& tag) noexcept
            {
                return typeid(VertexBuffer).name() + tag;
            }

            void Bind(Graphics& graphics) noexcept override;

        protected:
            UINT m_stride;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
            const std::string m_tag;
    };
}