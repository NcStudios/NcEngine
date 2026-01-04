#include "CascadedShadowMap.h"
  #include <algorithm>
  #include <cmath>

  namespace nc::graphics
  {

  CascadedShadowMap::CascadedShadowMap(const CascadeShadowConfig& config)
      : m_config{config}
  {
  }

  void CascadedShadowMap::CalculateSplitDepths(float nearClip, float shadowDistance)
  {
      m_splitDepths[0] = nearClip;

      for (uint32_t i = 1; i <= m_config.cascadeCount; ++i)
      {
          const float p = static_cast<float>(i) / static_cast<float>(m_config.cascadeCount);

          // Logarithmic distribution - better quality near camera
          const float logSplit = nearClip * std::pow(shadowDistance / nearClip, p);

          // Uniform distribution - even coverage
          const float uniformSplit = nearClip + (shadowDistance - nearClip) * p;

          // PSSM practical split scheme
          m_splitDepths[i] = m_config.splitLambda * logSplit +
                            (1.0f - m_config.splitLambda) * uniformSplit;
      }
  }

  void CascadedShadowMap::Update(
      DirectX::FXMMATRIX invViewProj,
      DirectX::FXMVECTOR lightDirection,
      float cameraNear,
      float cameraFar)
  {
      const float shadowDistance = std::min(m_config.shadowDistance, cameraFar);
      CalculateSplitDepths(cameraNear, shadowDistance);

      const auto lightDir = DirectX::XMVector3Normalize(lightDirection);

      for (uint32_t i = 0; i < m_config.cascadeCount; ++i)
      {
          const float cascadeNear = m_splitDepths[i];
          const float cascadeFar = m_splitDepths[i + 1];

          auto frustumCorners = CalculateFrustumCornersWorldSpace(
              invViewProj, cascadeNear, cascadeFar
          );

          // Calculate frustum center
          DirectX::XMVECTOR frustumCenter = DirectX::XMVectorZero();
          for (const auto& corner : frustumCorners.corners)
          {
              frustumCenter = DirectX::XMVectorAdd(frustumCenter, corner);
          }
          frustumCenter = DirectX::XMVectorScale(frustumCenter, 1.0f / 8.0f);

          // Calculate sphere radius for stable projection
          float radius = 0.0f;
          for (const auto& corner : frustumCorners.corners)
          {
              const float dist = DirectX::XMVectorGetX(
                  DirectX::XMVector3Length(DirectX::XMVectorSubtract(corner, frustumCenter))
              );
              radius = std::max(radius, dist);
          }
          radius = std::ceil(radius * 16.0f) / 16.0f;  // Quantize for stability

          // Light view matrix
          const auto lightPos = DirectX::XMVectorSubtract(
              frustumCenter,
              DirectX::XMVectorScale(lightDir, radius)
          );

          DirectX::XMVECTOR upDir = DirectX::g_XMIdentityR1;
          if (std::abs(DirectX::XMVectorGetY(lightDir)) > 0.99f)
          {
              upDir = DirectX::g_XMIdentityR2;
          }

          const auto lightView = DirectX::XMMatrixLookAtRH(
              lightPos, frustumCenter, upDir
          );

          // Orthographic projection encompassing frustum
          const auto lightProjection = DirectX::XMMatrixOrthographicRH(
              radius * 2.0f, radius * 2.0f, 0.0f, radius * 2.0f
          );

          auto cascadeViewProj = lightView * lightProjection;

          if (m_config.stabilizeCascades)
          {
              StabilizeCascadeMatrix(cascadeViewProj);
          }

          m_cascadeData[i].viewProjection = cascadeViewProj;
          m_cascadeData[i].splitDepth = cascadeFar;
          m_cascadeData[i].texelSize = (radius * 2.0f) / m_config.shadowMapResolution;
      }
  }

  auto CascadedShadowMap::CalculateFrustumCornersWorldSpace(
      DirectX::FXMMATRIX invViewProj,
      float nearDepth,
      float farDepth) const -> FrustumCorners
  {
      // NDC corners (RH: z goes from 0 to 1 in Vulkan/DX12)
      constexpr std::array<DirectX::XMVECTOR, 8> ndcCorners = {
          DirectX::XMVECTOR{-1.0f,  1.0f, 0.0f, 1.0f},
          DirectX::XMVECTOR{ 1.0f,  1.0f, 0.0f, 1.0f},
          DirectX::XMVECTOR{ 1.0f, -1.0f, 0.0f, 1.0f},
          DirectX::XMVECTOR{-1.0f, -1.0f, 0.0f, 1.0f},
          DirectX::XMVECTOR{-1.0f,  1.0f, 1.0f, 1.0f},
          DirectX::XMVECTOR{ 1.0f,  1.0f, 1.0f, 1.0f},
          DirectX::XMVECTOR{ 1.0f, -1.0f, 1.0f, 1.0f},
          DirectX::XMVECTOR{-1.0f, -1.0f, 1.0f, 1.0f}
      };

      FrustumCorners result;
      for (size_t i = 0; i < 8; ++i)
      {
          auto corner = DirectX::XMVector4Transform(ndcCorners[i], invViewProj);
          corner = DirectX::XMVectorDivide(corner, DirectX::XMVectorSplatW(corner));
          result.corners[i] = corner;
      }

      // Interpolate near/far planes to desired depths
      for (size_t i = 0; i < 4; ++i)
      {
          const auto ray = DirectX::XMVectorSubtract(result.corners[i + 4], result.corners[i]);
          result.corners[i] = DirectX::XMVectorAdd(
              result.corners[i],
              DirectX::XMVectorScale(ray, nearDepth)
          );
          result.corners[i + 4] = DirectX::XMVectorAdd(
              result.corners[i],
              DirectX::XMVectorScale(ray, farDepth)
          );
      }

      return result;
  }

  void CascadedShadowMap::StabilizeCascadeMatrix(DirectX::XMMATRIX& viewProj) const
  {
      // Snap to shadow map texel grid to prevent shimmer
      DirectX::XMVECTOR origin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
      origin = DirectX::XMVector4Transform(origin, viewProj);
      origin = DirectX::XMVectorScale(origin, m_config.shadowMapResolution * 0.5f);

      const auto rounded = DirectX::XMVectorRound(origin);
      auto offset = DirectX::XMVectorSubtract(rounded, origin);
      offset = DirectX::XMVectorScale(offset, 2.0f / m_config.shadowMapResolution);
      offset = DirectX::XMVectorSetZ(offset, 0.0f);
      offset = DirectX::XMVectorSetW(offset, 0.0f);

      viewProj.r[3] = DirectX::XMVectorAdd(viewProj.r[3], offset);
  }

  } // namespace nc::graphics