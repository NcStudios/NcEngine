#pragma once

#include "Assets.h"

namespace nc
{
    const std::string SkyboxMeshPath = "skybox.nca";

    struct EnvironmentData
    {
        Vector3 cameraWorldPosition;
        uint32_t skyboxTextureIndex;
    };

    class Environment
    {
        public:
            Environment();
            void Set(const CubeMapFaces& skybox);
            void Set(const Vector3& cameraPosition);
            const EnvironmentData& Get() const;
            void Clear();
            bool UseSkybox();

        private:
            EnvironmentData m_environmentData;
            bool m_useSkybox;
    };
}