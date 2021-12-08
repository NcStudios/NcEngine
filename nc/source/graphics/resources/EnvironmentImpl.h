#pragma once

#include "graphics/Environment.h"

namespace nc
{
    const std::string SkyboxMeshPath = "skybox.nca";
    struct EnvironmentData
    {
        Vector3 cameraWorldPosition;
        uint32_t skyboxTextureIndex;
    };

    class EnvironmentImpl : public Environment
    {
        public:
            EnvironmentImpl();

            void SetSkybox(const CubeMapFaces& skybox) override;
            void SetCameraPosition(const Vector3& cameraPosition);
            const EnvironmentData& Get() const;
            void Clear();
            bool UseSkybox();
            
        private:
            EnvironmentData m_environmentData;
            bool m_useSkybox;
    };
}