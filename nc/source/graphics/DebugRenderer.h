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
                constexpr size_t CubeVertexCount = 24u;
                DebugTechnique::BindCommonResources();
                m_mesh.Bind();

                for(const auto& matrices : m_points)
                {
                    BindMatrices(matrices);
                    m_graphics->DrawIndexed(CubeVertexCount);
                }
            }

            static void ClearPoints() { DebugRenderer::m_instance->m_points.clear(); }
            static void ClearLines() { DebugRenderer::m_instance->m_lines.clear(); }

            static void AddPoint(const Vector3& position)
            {
                using namespace DirectX;

                auto modelView = XMMatrixScaling(0.5f, 0.5f, 0.5f) *
                                 XMMatrixTranslation(position.x, position.y, position.z) *
                                 DebugRenderer::m_instance->m_graphics->GetViewMatrix();

                auto mvp = modelView * DebugRenderer::m_instance->m_graphics->GetProjectionMatrix();
                DebugRenderer::m_instance->m_points.emplace_back(XMMatrixTranspose(modelView), XMMatrixTranspose(mvp));
            }

        private:
            static inline DebugRenderer* m_instance = nullptr;
            Graphics* m_graphics;
            Mesh m_mesh;
            std::vector<MvpMatrices> m_points;
            std::vector<DirectX::XMMATRIX> m_lines;
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