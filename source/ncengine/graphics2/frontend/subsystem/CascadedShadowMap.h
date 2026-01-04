#pragma once
  #include "ncmath/Vector.h"
  #include "graphics2/ShaderTypes.h"
  #include <DirectXMath.h>
  #include <array>
  #include <span>

  namespace nc::graphics
  {

  class CascadedShadowMap
  {
  public:
      explicit CascadedShadowMap(const CascadeShadowConfig& config);

      void Update(
          DirectX::FXMMATRIX invViewViewProj,
          DirectX::FXMVECTOR lightDirection,
          float cameraNear,
          float cameraFar
      );

      [[nodiscard]] auto GetCascadeCount() const -> uint32_t { return m_config.cascadeCount; }
      [[nodiscard]] auto GetCascadeData() const -> std::span<const CascadeData>
      {
          return std::span{m_cascadeData.data(), m_config.cascadeCount};
      }
      [[nodiscard]] auto GetConfig() const -> const CascadeShadowConfig& { return m_config; }

      void SetConfig(const CascadeShadowConfig& config) { m_config = config; }

  private:
      struct FrustumCorners
      {
          std::array<DirectX::XMVECTOR, 8> corners;
      };

      void CalculateSplitDepths(float nearClip, float shadowDistance);

      [[nodiscard]] auto CalculateFrustumCornersWorldSpace(
          DirectX::FXMMATRIX invViewProj,
          float nearDepth,
          float farDepth
      ) const -> FrustumCorners;

      void StabilizeCascadeMatrix(DirectX::XMMATRIX& viewProj) const;

      CascadeShadowConfig m_config;
      std::array<float, MaxCascadeCount + 1> m_splitDepths{};
      std::array<CascadeData, MaxCascadeCount> m_cascadeData{};
  };

  } // namespace nc::graphics