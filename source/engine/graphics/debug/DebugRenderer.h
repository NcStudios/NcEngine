#pragma once
#include "debug/Serialize.h"

#ifdef NC_DEBUG_RENDERING_ENABLED

#include <vector>
#include <iostream>
namespace nc::graphics
{
    struct DebugData
    {
        DebugData()
        : points{},
          lines{},
          planes{}
        {}

        std::vector<DirectX::XMMATRIX> points;
        std::vector<DirectX::XMMATRIX> lines;
        std::vector<DirectX::XMMATRIX> planes;
    };

    class DebugRenderer
    {
        public:
            DebugRenderer()
                : m_debugData{}
            {
                DebugRenderer::m_instance = this;
            }

            static DebugData* GetData()
            {
                return &DebugRenderer::m_instance->m_debugData;
            }

            static void ClearPoints() { DebugRenderer::m_instance->m_debugData.points.clear(); }
            static void ClearLines() { DebugRenderer::m_instance->m_debugData.lines.clear(); }
            static void ClearPlanes() { DebugRenderer::m_instance->m_debugData.planes.clear(); }

            static void AddPoint(const Vector3& position)
            {
                using namespace DirectX;
                auto model = XMMatrixScaling(0.1f, 0.1f, 0.1f) *
                             XMMatrixTranslation(position.x, position.y, position.z);

                DebugRenderer::m_instance->m_debugData.points.emplace_back(model);
            }

            static void AddLine(const Vector3& positionStart, const Vector3& positionEnd)
            {
                using namespace DirectX;
                auto start_v = XMLoadVector3(&positionStart);
                auto end_v = XMLoadVector3(&positionEnd);
                auto distance_v = XMVector3Length(XMVectorSubtract(end_v, start_v));
                float distance = XMVectorGetX(distance_v);
                auto line_v = XMVector3Normalize(XMVectorSubtract(end_v, start_v));
                auto orthogonal_v = XMVector3Normalize(XMVector3Cross(line_v,  g_XMIdentityR0));
                auto angle_v = XMVector3AngleBetweenNormals(line_v, g_XMIdentityR0);
                auto midpoint_v = XMVectorAdd(start_v, XMVectorMultiply(line_v, XMVectorScale(distance_v, 0.5)));
                float angle = DirectX::XMVectorGetW(angle_v);
                auto rotationMatrix = XMMatrixRotationNormal(orthogonal_v, -angle);
                auto model = XMMatrixScaling(distance, 0.01f, 0.01f) *
                             rotationMatrix * 
                             XMMatrixTranslationFromVector(midpoint_v);

                DebugRenderer::m_instance->m_debugData.lines.emplace_back(model);
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

                DebugRenderer::m_instance->m_debugData.planes.emplace_back(scale_m * rot_m * trans_m);
            }

        private:
            static inline DebugRenderer* m_instance = nullptr;
            DebugData m_debugData;
            std::string m_mesh;

    };
}
#endif