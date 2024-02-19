#pragma once

#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/UniformBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "utility/Signal.h"

#include "ncmath/Vector.h"

namespace nc::graphics
{
struct CameraState;

#ifdef _MSC_VER
#pragma warning( disable : 4324 ) // Justification: We want to be explicit about alignment for the GPU buffer structs.
#endif
struct EnvironmentData
{
    alignas(16)Vector4 cameraWorldPosition = Vector4(0.0f, 4.0f, -6.4f, 0.0f);
    alignas(16)uint32_t skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
};
#ifdef _MSC_VER
#pragma warning( default  : 4324 )
#endif

struct EnvironmentState
{
    bool useSkybox;
};

class EnvironmentSystem
{
    public:
        EnvironmentSystem(ShaderResourceBus* shaderResourceBus);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const CameraState& cameraState, uint32_t currentFrame) -> EnvironmentState;

    private:
        EnvironmentData m_environmentData;
        UniformBufferHandle m_environmentDataBuffer;
        bool m_useSkybox;
};
} // namespace nc::graphics