#pragma once

/** @todo This class will probably be useful from time to time. It
 *  will need to be Vulkan compatible. We may also want this #define
 *  to be a build option in the future. */
//#define NC_DEBUG_RENDERING
#undef NC_DEBUG_RENDERING

#ifdef NC_DEBUG_RENDERING

#include "graphics/MvpMatrices.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "graphics/techniques/DebugTechnique.h"
#include "graphics/techniques/TechniqueManager.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    class DebugRenderer
    {
        public:
            DebugRenderer(Graphics* graphics)
                : m_graphics{graphics},
                  m_mesh{},
                  m_points{},
                  m_lines{},
                  m_planes{},
                  m_pixelCBuff{std::make_unique<d3dresource::PixelConstantBuffer<graphics::MvpMatrices>>(2u)},
                  m_vertexCBuff{std::make_unique<d3dresource::VertexConstantBuffer<graphics::MvpMatrices>>(0u)}
            {
                DebugRenderer::m_instance = this;
                TechniqueManager::GetTechnique<DebugTechnique>();

                LoadMeshAsset("project/assets/mesh/cube.nca");
                m_mesh = Mesh{"project/assets/mesh/cube.nca"};
            }

            void Render()
            {
                constexpr size_t CubeIndexCount = 36u;
                DebugTechnique::BindCommonResources();
                m_mesh.Bind();

                for(const auto& matrices : m_points)
                {
                    BindMatrices(matrices);
                    m_graphics->DrawIndexed(CubeIndexCount);
                }

                const auto& view = m_graphics->GetViewMatrix();
                const auto& projection = m_graphics->GetProjectionMatrix();

                for(const auto& matrix : m_planes)
                {
                    auto modelView = matrix * view;
                    auto mvp = modelView * projection;
                    BindMatrices({XMMatrixTranspose(modelView), XMMatrixTranspose(mvp)});
                    m_graphics->DrawIndexed(CubeIndexCount);
                }
            }

            static void ClearPoints() { DebugRenderer::m_instance->m_points.clear(); }
            static void ClearLines() { DebugRenderer::m_instance->m_lines.clear(); }
            static void ClearPlanes() { DebugRenderer::m_instance->m_planes.clear(); }

            static void AddPoint(const Vector3& position)
            {
                using namespace DirectX;

                auto modelView = XMMatrixScaling(0.1f, 0.1f, 0.1f) *
                                 XMMatrixTranslation(position.x, position.y, position.z) *
                                 DebugRenderer::m_instance->m_graphics->GetViewMatrix();

                auto mvp = modelView * DebugRenderer::m_instance->m_graphics->GetProjectionMatrix();
                DebugRenderer::m_instance->m_points.emplace_back(XMMatrixTranspose(modelView), XMMatrixTranspose(mvp));
            }

            static void AddPlane(const Vector3& normal, float d)
            {
                using namespace DirectX;

                auto trans = normal * d;
                auto trans_m = XMMatrixTranslation(trans.x, trans.y, trans.z);
                auto scale_m = XMMatrixScaling(100.0f, 0.01f, 100.0f);

                auto normal_v = XMLoadVector3(&normal);
                auto angle_v = XMVector3AngleBetweenVectors(normal_v, g_XMIdentityR1);
                auto axis_v = XMVector3Cross(normal_v, g_XMIdentityR1);
                auto rot_m = XMMatrixRotationAxis(axis_v, XMVectorGetX(angle_v));

                DebugRenderer::m_instance->m_planes.emplace_back(scale_m * rot_m * trans_m);
            }

        private:
            static inline DebugRenderer* m_instance = nullptr;
            Graphics* m_graphics;
            Mesh m_mesh;
            std::vector<MvpMatrices> m_points;
            std::vector<DirectX::XMMATRIX> m_lines;
            std::vector<DirectX::XMMATRIX> m_planes;
            std::unique_ptr<d3dresource::PixelConstantBuffer<MvpMatrices>> m_pixelCBuff;
            std::unique_ptr<d3dresource::VertexConstantBuffer<MvpMatrices>> m_vertexCBuff;

            void BindMatrices(const graphics::MvpMatrices& matrices)
            {
                m_vertexCBuff->Update(matrices);
                m_vertexCBuff->Bind();
                m_pixelCBuff->Update(matrices);
                m_pixelCBuff->Bind();
            }
    };
}
#endif